"""C/C++ と .NET の期待確認コメントから、設計上の回数を集計する。"""

import argparse
import codecs
import json
from pathlib import Path
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
TAG_START = re.compile(r"\[(?:Pre-Assert確認|確認|状態|手順|Pre-Assert手順|サブ手順)")
TAG = re.compile(r"\[([^\]]*)\]")
TEST = re.compile(r"\b(?:TEST(?:_[FP])?|TYPED_TEST(?:_P)?)\s*\(|^\s*\[(?:Fact|Theory)\b|^\s*(?:public|private|protected|internal|void|async|Task)\b")
# test_subprocedures.py がサブ手順参照を展開した範囲の開始と終了。
EXPANSION = re.compile(r"サブ手順展開\s+名前=(\S+)\s+回数=(.+)")
EXPANSION_END = "サブ手順展開終了"
# 節の中の並び順。テスト本体の前の参照は先頭、後の参照は末尾に置く。
PHASE_ORDER = {"前": 0, "Arrange": 1, "Act": 2, "Pre-Assert": 3, "Assert": 4, "後": 5}
# 参照を書いたフェーズごとに、親項目を必ず出す節。
HOME_SECTION = {"前": "state", "Arrange": "state", "Act": "act", "Pre-Assert": "act", "Assert": "check", "後": "act"}
PARENT_TEXT = {"state": "にて以下を準備する。", "act": "を実施する。", "check": "にて以下を確認する。"}


@dataclass
class Entry:
    """節に表示する項目。name があればサブ手順参照の親項目で、children に展開した項目を持つ。"""
    phase: str
    section: str = ""
    text: str = ""
    name: str = ""
    count: str = ""
    children: list = field(default_factory=list)

    def visible(self, section):
        if not self.name:
            return self.section == section
        return HOME_SECTION[self.phase] == section or any(child.visible(section) for child in self.children)


@dataclass
class Cycle:
    entries: list = field(default_factory=list)
    counts: dict = field(default_factory=lambda: {"正常系": 0, "異常系": 0})


def count_label(count, expression, param_count, partial, value):
    """回数の表記を返す。"""
    compact = "".join(expression.split())
    if partial:
        return f"〔未評価: {compact}〕"
    if count.uses_param:
        return f"〔{value} 回: {compact}、PARAM={param_count}〕"
    return f"〔{value} 回〕"


def render_entries(entries, section, depth=0):
    lines = []
    for entry in entries:
        if not entry.visible(section):
            continue
        indent = "    " * depth
        if not entry.name:
            lines.append(indent + entry.text)
            continue
        children = [child for child in entry.children if child.visible(section)]
        # 子項目がない親項目は、どの節でも実施したことだけを示す。
        phrase = PARENT_TEXT[section] if children else PARENT_TEXT["act"]
        lines.append(f"{indent}- サブ手順 {entry.name} {phrase}{entry.count}")
        lines.extend(render_entries(children, section, depth + 1))
    return lines


def render_summary(source, parameterized=False, param_count=None, partial=False, test_id="<stdin>", counts=None):
    parsed = list(source_lines(source))
    code = "\n".join(line for line, _ in parsed)
    parameterized = parameterized or bool(re.search(r"\bTEST_P\s*\(|\[Theory\b", code))
    if param_count is not None and (not isinstance(param_count, int) or param_count < 0):
        raise SummaryError("レコード数には非負整数が必要です")
    cycles = {1: Cycle()}
    current = 1
    desc = []
    found_test = False
    # テスト本体の後の参照は、最後のサイクルが確定してから末尾に置く。
    after = []
    phase = "前"
    depth = 0
    opened = False
    # 展開中のサブ手順参照の親項目と、その参照の回数の値。
    stack = []
    factors = []
    for number, ((line, comment), original) in enumerate(zip(parsed, source.splitlines()), 1):
        if not found_test:
            if TEST.search(line):
                found_test = True
                phase = "Arrange"
            elif stack or re.search(r"\[サブ手順(?:参照|展開)\b", comment):
                pass
            elif original.lstrip().startswith("//") and not TAG_START.search(comment):
                text = comment.lstrip("/ ").rstrip()
                if text:
                    desc.append(text)
            else:
                desc = []
        marker = PHASE.fullmatch(comment) if not line.strip() else None
        if marker and phase != "後":
            current = max(current, int(marker.group(2) or 1))
            phase = marker.group(1)
        cycle = cycles.setdefault(current, Cycle())
        starts = list(TAG_START.finditer(comment))
        tags = [tag for tag in TAG.finditer(comment)
                if any(tag.start() == start.start() for start in starts)]

        def add(entry):
            (stack[-1].children if stack else after if phase == "後" else cycle.entries).append(entry)

        try:
            if any(not any(tag.start() == start.start() for tag in tags) for start in starts):
                raise SummaryError("タグの閉じ括弧がありません")
            for index, tag in enumerate(tags):
                name = tag.group(1)
                end = tags[index + 1].start() if index + 1 < len(tags) else len(comment)
                text = comment[tag.end():end].strip()
                match = CHECK.fullmatch(name)
                expansion = EXPANSION.fullmatch(name)
                if expansion:
                    expression = expansion.group(2).strip()
                    count = CountExpression(expression)
                    if count.uses_param and not parameterized:
                        raise SummaryError("通常テストでは PARAM を使用できません")
                    if count.uses_param and param_count is None:
                        raise SummaryError("PARAM のレコード数を確定できません")
                    value = None if partial else count.evaluate(param_count)
                    shown = "".join(expression.split()) != "1" if partial else value != 1
                    entry = Entry(phase, name=expansion.group(1),
                                  count=count_label(count, expression, param_count, partial, value) if shown else "")
                    add(entry)
                    stack.append(entry)
                    factors.append(value)
                elif name == EXPANSION_END:
                    if not stack:
                        raise SummaryError("サブ手順展開の終了が対応していません")
                    stack.pop()
                    factors.pop()
                elif match:
                    item = LIST.fullmatch(text)
                    if not item:
                        raise SummaryError("確認タグには箇条書きの説明文が必要です")
                    text = text[:item.start(1)] + f"【{match.group(2)}】" + item.group(1)
                    expression = match.group(3)
                    count = CountExpression(expression if expression is not None else "1")
                    if count.uses_param and not parameterized:
                        raise SummaryError("通常テストでは PARAM を使用できません")
                    if count.uses_param and param_count is None:
                        raise SummaryError("PARAM のレコード数を確定できません")
                    value = None if partial else count.evaluate(param_count)
                    if value is not None:
                        total = value
                        for factor in factors:
                            total *= factor
                        cycle.counts[match.group(2)] += total
                    if stack:
                        # サブ手順内の回数は 1 回あたりの値で、1 以外のときだけ表記する。
                        shown = expression is not None and "".join(expression.split()) != "1" if partial else value != 1
                    else:
                        shown = expression is not None
                    if shown:
                        text += count_label(count, expression or "1", param_count, partial, value)
                    add(Entry(phase, "check", text))
                elif name.startswith("サブ手順"):
                    raise SummaryError("サブ手順は定義一覧を使って解決してから集計してください")
                elif name.startswith(("確認", "Pre-Assert確認")):
                    raise SummaryError(f"不正な確認タグ: [{name}]")
                elif text:
                    if name == "状態確認":
                        item = LIST.fullmatch(text)
                        add(Entry(phase, "check", f"- ({item.group(1)})" if item else text))
                    elif name == "状態":
                        add(Entry(phase, "state", text))
                    elif name in {"手順", "Pre-Assert手順"}:
                        add(Entry(phase, "act", text))
        except (SummaryError, ValueError) as error:
            raise SummaryError(f"{test_id}:抽出コード:{number}: {error}") from error
        # テスト本体の終了括弧より後は、テスト本体の後の参照として扱う。
        if found_test and phase != "後":
            depth += line.count("{") - line.count("}")
            opened = opened or depth > 0
            if opened and depth <= 0:
                phase = "後"
    if stack:
        raise SummaryError(f"{test_id}: サブ手順展開の終了がありません")
    totals = {category: sum(c.counts[category] for c in cycles.values()) for category in ("正常系", "異常系")}
    if counts is not None:
        counts.update({category: None if partial else value for category, value in totals.items()})
    if not desc and not after and not any(c.entries for c in cycles.values()):
        return ""
    out = ["## テスト項目の詳細\n"]
    if desc:
        out.append("\n" + "\n".join(desc) + "\n")
    multi = current > 1
    totals = {"正常系": 0, "異常系": 0}
    for number in range(1, current + 1):
        cycle = cycles.get(number, Cycle())
        # 同じフェーズの中はテスト コード上の順を保つ。
        entries = sorted(cycle.entries + (after if number == current else []), key=lambda e: PHASE_ORDER[e.phase])
        suffix = f"_{number}" if multi else ""
        out.append(f"\n### 状態{suffix}\n\n" + ("\n".join(render_entries(entries, "state")) or "特記すべき状態なし") + "\n")
        out.append(f"\n### 手順{suffix}\n\n" + ("\n".join(render_entries(entries, "act")) or "なし") + "\n")
        for category in totals:
            totals[category] += cycle.counts[category]
        header = "未評価" if partial else ", ".join(f"{k}:{v}" for k, v in cycle.counts.items() if v) or "0"
        out.append(f"\n### 確認{suffix} ({header})\n\n")
        out.append(("\n".join(render_entries(entries, "check")) or "なし") + "\n")
    if multi:
        header = "未評価" if partial else ", ".join(f"{k}:{v}" for k, v in totals.items() if v) or "0"
        out.append(f"\n### 確認件数合計 ({header})\n")
    return "".join(out)


def main(language):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--param-count", type=int)
    parser.add_argument("--parameterized", action="store_true")
    parser.add_argument("--partial", action="store_true")
    parser.add_argument("--test-id", default="<stdin>")
    parser.add_argument("--encoding", default="utf-8")
    parser.add_argument("--counts-output")
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
        counts = {}
        summary = "" if args.code_only else render_summary(
            source, args.parameterized, args.param_count, args.partial, args.test_id, counts)
        if args.counts_output:
            Path(args.counts_output).write_text(json.dumps(None if args.partial else counts, ensure_ascii=False), encoding="utf-8")
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
