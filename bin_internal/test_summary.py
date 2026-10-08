"""C/C++ と .NET の期待確認コメントから、設計上の回数を集計する。"""

import argparse
import codecs
from dataclasses import dataclass, field
import re
import sys


class SummaryError(ValueError):
    """確認タグまたは回数式の不備。"""


def text_encoding(name):
    """VS Code の文字コード指定を Python の codec 名に変換する。"""
    aliases = {"utf8bom": "utf-8-sig", "shiftjis": "cp932"}
    return codecs.lookup(aliases.get(name.lower(), name)).name


class CountExpression:
    """非負整数、PARAM、加算、乗算、括弧だけの再帰下降パーサー。"""

    def __init__(self, source):
        self.tokens = []
        pos = 0
        while pos < len(source):
            if source[pos].isspace():
                pos += 1
                continue
            match = re.match(r"[0-9]+|PARAM\b|[+*()]", source[pos:])
            if not match:
                raise SummaryError(f"回数式の不正な文字: {source[pos:]!r}")
            self.tokens.append(match.group())
            pos += len(match.group())
        self.pos = 0
        try:
            self.tree = self.expression()
        except RecursionError as error:
            raise SummaryError("回数式の括弧が深すぎます") from error
        if self.pos != len(self.tokens):
            raise SummaryError("回数式に余分なトークンがあります")
        self.uses_param = "PARAM" in self.tokens

    def take(self, token):
        if self.pos < len(self.tokens) and self.tokens[self.pos] == token:
            self.pos += 1
            return True
        return False

    def expression(self):
        node = self.term()
        while self.take("+"):
            node = ("+", node, self.term())
        return node

    def term(self):
        node = self.factor()
        while self.take("*"):
            node = ("*", node, self.factor())
        return node

    def factor(self):
        if self.take("("):
            node = self.expression()
            if not self.take(")"):
                raise SummaryError("回数式の閉じ括弧がありません")
            return node
        if self.take("PARAM"):
            return "PARAM"
        if self.pos < len(self.tokens) and self.tokens[self.pos].isdigit():
            token = self.tokens[self.pos]
            self.pos += 1
            try:
                return int(token)
            except ValueError as error:
                raise SummaryError("回数式の整数が長すぎます") from error
        raise SummaryError("回数式に非負整数、PARAM または括弧が必要です")

    def evaluate(self, param_count=None):
        if self.uses_param and param_count is None:
            raise SummaryError("PARAM のレコード数を確定できません")
        # 左結合の長い式も Python の再帰上限に依存せず評価する。
        pending = [(self.tree, False)]
        values = []
        while pending:
            node, visited = pending.pop()
            if isinstance(node, int):
                values.append(node)
            elif node == "PARAM":
                values.append(param_count)
            elif visited:
                right, left = values.pop(), values.pop()
                values.append(left + right if node[0] == "+" else left * right)
            else:
                pending.extend([(node, True), (node[2], False), (node[1], False)])
        return values[0]


def source_lines(source):
    """文字列とブロックコメントを除き、コードと // コメントを行ごとに返す。"""
    block = False
    quote = None
    raw_end = None
    verbatim = False
    for line in source.splitlines(keepends=True):
        code = []
        comment = ""
        pos = 0
        while pos < len(line):
            if raw_end:
                end = line.find(raw_end, pos)
                if end < 0:
                    break
                pos = end + len(raw_end)
                raw_end = None
            elif block:
                end = line.find("*/", pos)
                if end < 0:
                    break
                pos = end + 2
                block = False
            elif quote:
                if verbatim and line.startswith('""', pos):
                    pos += 2
                elif not verbatim and line[pos] == "\\":
                    pos += 2
                elif line[pos] == quote:
                    quote = None
                    verbatim = False
                    pos += 1
                else:
                    pos += 1
            elif line.startswith("//", pos):
                comment = line[pos + 2:].rstrip("\r\n")
                break
            elif line.startswith("/*", pos):
                code.append(" ")
                block = True
                pos += 2
            else:
                raw = re.match(r'(?:u8|u|U|L)?R"([^ ()\\\t\r\n]{0,16})\(', line[pos:])
                cs_raw = re.match(r'\$*("{3,})', line[pos:])
                if raw:
                    raw_end = ")" + raw.group(1) + '"'
                    code.append(" ")
                    pos += len(raw.group())
                elif cs_raw:
                    raw_end = cs_raw.group(1)
                    code.append(" ")
                    pos += len(cs_raw.group())
                elif line.startswith('@"', pos):
                    quote, verbatim = '"', True
                    code.append(" ")
                    pos += 2
                elif line[pos] in "\"'":
                    quote = line[pos]
                    code.append(" ")
                    pos += 1
                else:
                    code.append(line[pos])
                    pos += 1
        yield "".join(code), comment


PHASE = re.compile(r"^\s*(Arrange|Pre-Assert|Act|Assert)(?:_([0-9]+))?\s*$")
CHECK = re.compile(r"(Pre-Assert確認|確認)_(正常系|異常系)(?:\s+回数\s*=\s*(.+))?")
LIST = re.compile(r"^(?:[-*+]\s+|[0-9]+\.\s+)(\S.*)$")
TAG_START = re.compile(r"\[(?:Pre-Assert確認|確認|状態|手順|Pre-Assert手順)")
TAG = re.compile(r"\[([^\]]*)\]")
TEST = re.compile(r"\b(?:TEST(?:_[FP])?|TYPED_TEST(?:_P)?)\s*\(|^\s*\[(?:Fact|Theory)\b|^\s*(?:public|private|protected|internal|void|async|Task)\b")


@dataclass
class Cycle:
    state: list = field(default_factory=list)
    state_check: list = field(default_factory=list)
    act: list = field(default_factory=list)
    pre_step: list = field(default_factory=list)
    pre_check: list = field(default_factory=list)
    check: list = field(default_factory=list)
    counts: dict = field(default_factory=lambda: {"正常系": 0, "異常系": 0})


def render_summary(source, parameterized=False, param_count=None, partial=False, test_id="<stdin>"):
    parsed = list(source_lines(source))
    code = "\n".join(line for line, _ in parsed)
    parameterized = parameterized or bool(re.search(r"\bTEST_P\s*\(|\[Theory\b", code))
    if param_count is not None and (not isinstance(param_count, int) or param_count < 0):
        raise SummaryError("レコード数には非負整数が必要です")
    cycles = {1: Cycle()}
    current = 1
    desc = []
    found_test = False
    for number, ((line, comment), original) in enumerate(zip(parsed, source.splitlines()), 1):
        if not found_test:
            if TEST.search(line):
                found_test = True
            elif original.lstrip().startswith("//"):
                text = comment.lstrip("/ ").rstrip()
                if text:
                    desc.append(text)
            else:
                desc = []
        phase = PHASE.fullmatch(comment) if not line.strip() else None
        if phase:
            current = max(current, int(phase.group(2) or 1))
        cycle = cycles.setdefault(current, Cycle())
        starts = list(TAG_START.finditer(comment))
        tags = [tag for tag in TAG.finditer(comment)
                if any(tag.start() == start.start() for start in starts)]
        try:
            if any(not any(tag.start() == start.start() for tag in tags) for start in starts):
                raise SummaryError("タグの閉じ括弧がありません")
            for index, tag in enumerate(tags):
                name = tag.group(1)
                end = tags[index + 1].start() if index + 1 < len(tags) else len(comment)
                text = comment[tag.end():end].strip()
                match = CHECK.fullmatch(name)
                if match:
                    item = LIST.fullmatch(text)
                    if not item:
                        raise SummaryError("確認タグには箇条書きの説明文が必要です")
                    expression = match.group(3)
                    count = CountExpression(expression if expression is not None else "1")
                    if count.uses_param and not parameterized:
                        raise SummaryError("通常テストでは PARAM を使用できません")
                    if count.uses_param and param_count is None:
                        raise SummaryError("PARAM のレコード数を確定できません")
                    value = None if partial else count.evaluate(param_count)
                    if value is not None:
                        cycle.counts[match.group(2)] += value
                    if expression is not None:
                        compact = "".join(expression.split())
                        if partial:
                            text += f"〔未評価: {compact}〕"
                        elif count.uses_param:
                            text += f"〔{value} 回: {compact}、PARAM={param_count}〕"
                        else:
                            text += f"〔{value} 回〕"
                    (cycle.pre_check if match.group(1).startswith("Pre-") else cycle.check).append(text)
                elif name.startswith(("確認", "Pre-Assert確認")):
                    raise SummaryError(f"不正な確認タグ: [{name}]")
                elif text:
                    if name == "状態確認":
                        item = LIST.fullmatch(text)
                        cycle.state_check.append(f"- ({item.group(1)})" if item else text)
                    elif name == "状態":
                        cycle.state.append(text)
                    elif name == "手順":
                        cycle.act.append(text)
                    elif name == "Pre-Assert手順":
                        cycle.pre_step.append(text)
        except (SummaryError, ValueError) as error:
            raise SummaryError(f"{test_id}:抽出コード:{number}: {error}") from error
    if not desc and not any(c.state or c.state_check or c.act or c.pre_step or c.pre_check or c.check for c in cycles.values()):
        return ""
    out = ["## テスト項目\n"]
    if desc:
        out.append("\n" + "\n".join(desc) + "\n")
    multi = current > 1
    totals = {"正常系": 0, "異常系": 0}
    for number in range(1, current + 1):
        cycle = cycles.get(number, Cycle())
        suffix = f"_{number}" if multi else ""
        out.append(f"\n### 状態{suffix}\n\n" + "\n".join(cycle.state) + "\n")
        out.append(f"\n### 手順{suffix}\n\n" + "\n".join(cycle.act + cycle.pre_step) + "\n")
        for category in totals:
            totals[category] += cycle.counts[category]
        header = "未評価" if partial else ", ".join(f"{k}:{v}" for k, v in cycle.counts.items() if v) or "0"
        out.append(f"\n### 確認内容{suffix} ({header})\n\n")
        out.append("\n".join(cycle.state_check + cycle.pre_check + cycle.check) + "\n")
    if multi:
        header = "未評価" if partial else ", ".join(f"{k}:{v}" for k, v in totals.items() if v) or "0"
        out.append(f"\n### 確認件数合計 ({header})\n")
    out.append("----\n")
    return "".join(out)


def main(language):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--param-count", type=int)
    parser.add_argument("--parameterized", action="store_true")
    parser.add_argument("--partial", action="store_true")
    parser.add_argument("--test-id", default="<stdin>")
    parser.add_argument("--encoding", default="utf-8")
    output = parser.add_mutually_exclusive_group()
    output.add_argument("--summary-only", action="store_true")
    output.add_argument("--code-only", action="store_true")
    args = parser.parse_args()
    try:
        encoding = text_encoding(args.encoding)
    except LookupError as error:
        parser.error(str(error))
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdin.reconfigure(encoding=encoding)
        # BOM は各セクションの先頭に再挿入しない。
        sys.stdout.reconfigure(encoding="utf-8" if encoding == "utf-8-sig" else encoding)
        sys.stderr.reconfigure(encoding="utf-8")
    source = sys.stdin.read()
    try:
        summary = "" if args.code_only else render_summary(
            source, args.parameterized, args.param_count, args.partial, args.test_id)
    except SummaryError as error:
        print(f"[  FAILED  ] {error}", file=sys.stderr)
        return 1
    sys.stdout.write(summary)
    if not args.summary_only:
        if language == "dotnet":
            lines = source.splitlines(keepends=True)
            indent = min((len(line) - len(line.lstrip(" ")) for line in lines if line.strip()), default=0)
            source = "".join(line[indent:] if line.strip() else line for line in lines)
        sys.stdout.write(source)
    return 0
