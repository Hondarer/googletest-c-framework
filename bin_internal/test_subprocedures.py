"""名前付きサブ手順を解決し、テスト概要と元ソースの抜粋を生成する。"""

import argparse
from dataclasses import dataclass
from pathlib import Path
import re
import sys

from test_summary import CHECK, TAG, CountExpression, SummaryError, render_summary, source_lines, text_encoding

CONTROL = re.compile(r"\[(サブ手順参照|サブ手順終了|サブ手順)(?:\s+([^\]]*))?\]")
NAME = re.compile(r"[A-Za-z_][A-Za-z0-9_.:]*\Z")
CPP_TEST = re.compile(r"\b(?:TEST(?:_[FP])?|TYPED_TEST(?:_P)?)\s*\(\s*([\w]+)\s*,\s*([\w]+)\s*\)")
CS_TEST = re.compile(r"\[(?:Fact|Theory)(?:\s*\([^\]]*\))?\s*\]")


def attributes(text, reference=False):
    parts = list(re.finditer(r"(?:^|\s+)([^\s=]+)\s*=\s*", text or ""))
    if not parts or (text or "")[:parts[0].start()].strip():
        raise SummaryError("サブ手順には 名前=識別子 が必要です")
    result = {}
    for i, part in enumerate(parts):
        key = part.group(1)
        value = text[part.end():parts[i + 1].start() if i + 1 < len(parts) else len(text)].strip()
        if key not in ({"名前", "回数", "区分"} if reference else {"名前"}) or key in result:
            raise SummaryError(f"サブ手順の未知または重複した属性: {key}")
        result[key] = value
    if not NAME.fullmatch(result.get("名前", "")):
        raise SummaryError("サブ手順の名前が不正です")
    if reference:
        if "区分" in result and result["区分"] not in {"正常系", "異常系"}:
            raise SummaryError("サブ手順参照の区分は正常系または異常系です")
        CountExpression(result.get("回数", "1"))
    return result


def controls(code, comment, location):
    matches = list(CONTROL.finditer(comment))
    if "[サブ手順" in comment and not matches:
        raise SummaryError(f"{location}: サブ手順タグが不正です")
    if len(matches) > 1:
        raise SummaryError(f"{location}: サブ手順タグは 1 行に一つ記載してください")
    if not matches:
        return None
    match = matches[0]
    try:
        kind = match.group(1)
        if kind != "サブ手順参照" and (code.strip() or comment.strip() != match.group()):
            raise SummaryError("開始・終了タグは独立したコメント行に記載してください")
        if kind == "サブ手順終了":
            if match.group(2):
                raise SummaryError("サブ手順終了に属性は指定できません")
            attrs = {}
        else:
            attrs = attributes(match.group(2), kind == "サブ手順参照")
        if kind == "サブ手順参照":
            if comment.strip() != match.group():
                raise SummaryError("参照コメントにはサブ手順参照だけを記載してください")
            if any(CHECK.fullmatch(t.group(1)) for t in TAG.finditer(comment)):
                raise SummaryError("確認タグとサブ手順参照は併記できません")
        return kind, attrs
    except SummaryError as error:
        raise SummaryError(f"{location}: {error}") from error


def platform_lines(source, is_windows=None):
    """既存の PLATFORM_* 単独判定だけを評価し、元の行番号を保持する。"""
    lines = source.splitlines(keepends=True)
    if is_windows is None:
        return lines
    parsed = list(source_lines(source))
    stack = []
    active = True
    output = []
    for line, (code, _) in zip(lines, parsed):
        directive = re.match(r"\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)", code)
        if not directive:
            output.append(line if active else "\n")
            continue
        kind, expr = directive.groups()
        directive_active = active
        evaluated = stack[-1][1] if stack else False
        known = re.fullmatch(r"\s*defined\s*\(\s*PLATFORM_(WINDOWS|LINUX)\s*\)\s*", expr)
        if kind in {"if", "ifdef", "ifndef"}:
            enabled = bool(known) and ((known.group(1) == "WINDOWS") == bool(is_windows))
            stack.append([active, bool(known) and kind == "if", enabled])
            evaluated = stack[-1][1]
            active = active and (enabled if stack[-1][1] else True)
        elif stack:
            parent, evaluated, taken = stack[-1]
            if kind == "endif":
                active = parent
                stack.pop()
            elif kind == "else":
                active = parent and (not taken if evaluated else True)
                stack[-1][2] = True
            elif kind == "elif":
                if evaluated and known:
                    enabled = (known.group(1) == "WINDOWS") == bool(is_windows)
                    active = parent and not taken and enabled
                    stack[-1][2] = taken or enabled
                else:
                    stack[-1][1] = False
                    active = parent
        # 評価済み分岐のディレクティブだけを除く。
        visible = directive_active if kind in {"if", "ifdef", "ifndef", "endif"} else active
        output.append(line if visible and not evaluated else "\n")
    return output


@dataclass
class Fragment:
    path: Path
    start: int
    end: int
    source: str
    indent: str = ""
    parameterized: bool = False

    @property
    def location(self):
        return f"{self.path}:{self.start + 1}"


def test_ranges(source, language):
    """コメント・文字列を除いたコードの括弧でテスト範囲を求める。"""
    masked = "\n".join(code.rstrip("\r\n") for code, _ in source_lines(source))
    pattern = CPP_TEST if language == "c_cpp" else CS_TEST
    for match in pattern.finditer(masked):
        start = masked.count("\n", 0, match.start())
        opening = masked.find("{", match.end())
        if opening < 0:
            raise SummaryError(f"テスト本体の開始括弧がありません: {start + 1}")
        depth = 1
        end = opening + 1
        while end < len(masked) and depth:
            if masked[end] == "{":
                depth += 1
            elif masked[end] == "}":
                depth -= 1
            end += 1
        if depth:
            raise SummaryError(f"テスト本体の終了括弧がありません: {start + 1}")
        if language == "c_cpp":
            key = f"{match.group(1)}.{match.group(2)}"
            parameterized = masked[match.start():match.end()].startswith("TEST_P")
        else:
            signature = masked[match.end():opening]
            method = re.search(r"(\w+)\s*\([^()]*\)\s*(?:where\b.*)?$", signature, re.S)
            classes = list(re.finditer(r"\bclass\s+(\w+)", masked[:match.start()]))
            if not method or not classes:
                raise SummaryError(f"テスト メソッドを特定できません: {start + 1}")
            key = f"{classes[-1].group(1)}.{method.group(1)}"
            parameterized = "Theory" in match.group()
        yield key, start, masked.count("\n", 0, end - 1), parameterized


def dedent_source(source, indent):
    """文字列の内容を保持し、指定した外側の字下げだけを除く。"""
    # 字句解析で行頭がコードか複数行文字列の内容かを区別する。
    lines = source.splitlines(keepends=True)
    parsed = list(source_lines(source))
    output = []
    for line, (code, comment) in zip(lines, parsed):
        # 文字列継続行は解析結果から行頭の字下げが失われる。
        starts_as_code = code.startswith(indent) if indent else True
        if line.startswith(indent) and (starts_as_code or line.lstrip().startswith("//") and comment):
            output.append(line[len(indent):])
        else:
            output.append(line)
    return "".join(output)


class SourceIndex:
    def __init__(self, sources, language="c_cpp", is_windows=None):
        self.language = language
        self.definitions = {}
        self.tests = {}
        self.instantiations = []
        self.files = {}
        for path, source in sources.items():
            path = Path(path)
            lines = platform_lines(source, is_windows)
            selected = "".join(lines)
            parsed = list(source_lines(selected))
            original_parsed = list(source_lines(source))
            original_lines = source.splitlines(keepends=True)
            self.files[path] = lines
            markers = {}
            owner = [False] * len(lines)
            active = None
            for i, (code, comment) in enumerate(parsed):
                original_code, original_comment = original_parsed[i]
                original_item = controls(original_code, original_comment, f"{path}:{i + 1}")
                item = original_item if original_item and original_item[0] != "サブ手順参照" else controls(code, comment, f"{path}:{i + 1}")
                if item:
                    markers[i] = item
                    kind, attrs = item
                    if kind == "サブ手順":
                        if active:
                            raise SummaryError(f"{path}:{i + 1}: サブ手順の範囲が入れ子です")
                        active = (i, attrs["名前"])
                    elif kind == "サブ手順終了":
                        if not active:
                            raise SummaryError(f"{path}:{i + 1}: 対応するサブ手順がありません")
                        first, name = active
                        if name in self.definitions:
                            raise SummaryError(f"{path}:{first + 1}: サブ手順 {name} が重複しています: {self.definitions[name].location}")
                        indent = re.match(r"[ \t]*", original_lines[first]).group()
                        self.definitions[name] = Fragment(path, first + 1, i - 1, "".join(lines[first + 1:i]), indent)
                        for j in range(first, i + 1):
                            owner[j] = True
                        active = None
            if active:
                raise SummaryError(f"{path}:{active[0] + 1}: サブ手順終了がありません")
            ranges = list(test_ranges(selected, language))
            claimed = {}
            for key, first, last, parameterized in ranges:
                before = first
                while before > 0 and markers.get(before - 1, (None,))[0] == "サブ手順参照" and not parsed[before - 1][0].strip():
                    before -= 1
                # 直前の通常コメントも既存のテスト説明として保持する。
                desc = before
                while desc > 0 and lines[desc - 1].lstrip().startswith("//") and markers.get(desc - 1, (None,))[0] != "サブ手順参照":
                    desc -= 1
                after = last + 1
                while after < len(lines) and markers.get(after, (None,))[0] == "サブ手順参照" and not parsed[after][0].strip():
                    after += 1
                for j in list(range(before, first)) + list(range(last + 1, after)):
                    if j in claimed:
                        raise SummaryError(f"{path}:{j + 1}: 前後の所属が曖昧なサブ手順参照です ({claimed[j]}, {key})")
                    claimed[j] = key
                for j in range(desc, after):
                    owner[j] = True
                fragment = Fragment(path, desc, after - 1, "".join(lines[desc:after]), re.match(r"[ \t]*", lines[first]).group(), parameterized)
                self.tests.setdefault(key, []).append(fragment)
            for i, (code, comment) in enumerate(parsed):
                if not owner[i] and (markers.get(i, (None,))[0] == "サブ手順参照" or any(t.group(1).startswith(("確認", "Pre-Assert確認")) for t in TAG.finditer(comment))):
                    raise SummaryError(f"{path}:{i + 1}: テストまたはサブ手順に所属しない確認・参照です")
            # パラメーター登録コードは表示だけに用いる。
            if language == "c_cpp":
                masked = "\n".join(code.rstrip("\r\n") for code, _ in parsed)
                for match in re.finditer(r"\bINSTANTIATE_TEST_SUITE_P\s*\(\s*(\w+)\s*,\s*(\w+)\s*,", masked):
                    # 開始括弧から対応する終了括弧を求める。
                    depth = 0
                    for end in range(masked.find("(", match.start()), len(masked)):
                        if masked[end] == "(":
                            depth += 1
                        elif masked[end] == ")":
                            depth -= 1
                        if depth == 0:
                            break
                    first = masked.count("\n", 0, match.start())
                    last = masked.count("\n", 0, end)
                    self.instantiations.append((match.group(1), match.group(2), "".join(lines[first:last + 1])))
        try:
            self.validate()
        except RecursionError as error:
            raise SummaryError("サブ手順の参照が深すぎます") from error

    @classmethod
    def from_paths(cls, paths, language="c_cpp", is_windows=None, encoding="utf-8"):
        sources = {}
        pending = list(paths)
        while pending:
            path = Path(pending.pop()).resolve()
            if path in sources:
                continue
            builtin = Path(__file__).resolve().parents[1] / "libsrc/test_com/export_check.cc"
            text = path.read_text(encoding="utf-8" if path == builtin else encoding)
            sources[path] = text
            if "testing.expectExportNamesMatch" in text:
                builtin = Path(__file__).resolve().parents[1] / "libsrc/test_com/export_check.cc"
                if builtin != path:
                    pending.append(builtin)
            if language == "c_cpp":
                # コンパイル対象の相対 include だけをたどる。標準・外部ヘッダーは探索しない。
                for include in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"', text, re.M):
                    header = path.parent / include
                    if header.is_file() and header.suffix in {".h", ".hpp", ".hh"}:
                        pending.append(header)
        return cls(sources, language, is_windows)

    def validate(self):
        state = {}
        def visit(name, route):
            if name not in self.definitions:
                raise SummaryError(f"未定義のサブ手順: {' → '.join(route + [name])}")
            if state.get(name) == 1:
                raise SummaryError(f"サブ手順が循環しています: {' → '.join(route + [name])}")
            if state.get(name) == 2:
                return
            state[name] = 1
            fragment = self.definitions[name]
            for i, (code, comment) in enumerate(source_lines(fragment.source), fragment.start + 1):
                try:
                    for tag in TAG.finditer(comment):
                        check = CHECK.fullmatch(tag.group(1))
                        if check and CountExpression(check.group(3) or "1").uses_param:
                            raise SummaryError("サブ手順内では PARAM を使用できません")
                    item = controls(code, comment, f"{fragment.path}:{i}")
                    if item and item[0] == "サブ手順参照":
                        if CountExpression(item[1].get("回数", "1")).uses_param:
                            raise SummaryError("サブ手順内では PARAM を使用できません")
                        visit(item[1]["名前"], route + [name])
                except SummaryError as error:
                    raise SummaryError(f"{fragment.path}:{i}: {error}") from error
            # 不正な確認タグと説明文も未参照の定義を含めて検証する。
            checked = "\n".join("" if controls(code, comment, fragment.location) else original
                                for original, (code, comment) in zip(fragment.source.splitlines(), source_lines(fragment.source)))
            try:
                render_summary(checked, test_id=fragment.location)
            except SummaryError as error:
                match = re.search(r":抽出コード:(\d+): (.*)", str(error), re.S)
                if match:
                    raise SummaryError(f"{fragment.path}:{fragment.start + int(match.group(1))}: {match.group(2)}") from error
                raise
            state[name] = 2
        for name in self.definitions:
            visit(name, [])
        for fragments in self.tests.values():
            for fragment in fragments:
                for i, (code, comment) in enumerate(source_lines(fragment.source), fragment.start + 1):
                    item = controls(code, comment, f"{fragment.path}:{i}")
                    if item and item[0] == "サブ手順参照" and item[1]["名前"] not in self.definitions:
                        raise SummaryError(f"{fragment.path}:{i}: 未定義のサブ手順: {item[1]['名前']}")

    def expand(self, fragment, route, multiplier, reached, root=True, category=None):
        output = []
        local_cycle = 1
        in_definition = False
        for i, (original, (code, comment)) in enumerate(zip(fragment.source.splitlines(), source_lines(fragment.source)), fragment.start + 1):
            item = controls(code, comment, f"{fragment.path}:{i}")
            if root and item and item[0] == "サブ手順":
                in_definition = True
                continue
            if root and item and item[0] == "サブ手順終了":
                in_definition = False
                continue
            if in_definition:
                continue
            if item and item[0] == "サブ手順参照":
                name = item[1]["名前"]
                if name not in reached:
                    reached.append(name)
                factor = item[1].get("回数", "1")
                product = factor if multiplier == "1" else f"({multiplier})*({factor})"
                output.append(f"// [手順] - サブ手順 {name} を実施する。〔参照回数={product}〕")
                output.append(self.expand(self.definitions[name], route + [name], product, reached, False, item[1].get("区分", category)))
                continue
            if root:
                output.append(original)
                continue
            # サブ手順内部のサイクル番号は参照元を変更しない。
            phase = re.fullmatch(r"\s*(?:Arrange|Pre-Assert|Act|Assert)(?:_(\d+))?\s*", comment)
            if phase:
                local_cycle = max(local_cycle, int(phase.group(1) or 1))
            if phase or not comment:
                continue
            tags = [t for t in TAG.finditer(comment) if t.group(1).startswith(("確認", "Pre-Assert確認")) or t.group(1) in {"状態", "状態確認", "手順", "Pre-Assert手順"}]
            for n, tag in enumerate(tags):
                text = comment[tag.end():tags[n + 1].start() if n + 1 < len(tags) else len(comment)].strip()
                check = CHECK.fullmatch(tag.group(1))
                label = f"〔サブ手順 {' → '.join(route)}、内部サイクル={local_cycle}、参照回数={multiplier}〕"
                if check:
                    count = check.group(3) or "1"
                    expression = count if multiplier == "1" else f"({multiplier})*({count})"
                    output.append(f"// [{check.group(1)}_{category or check.group(2)} 回数={expression}] {text} {label}")
                elif tag.group(1) in {"状態", "状態確認", "手順", "Pre-Assert手順"}:
                    output.append(f"// [{tag.group(1)}] {text} {label}")
        return "\n".join(output) + "\n"

    def report(self, test_id, parameterized=False, param_count=None, partial=False, code_only=False, prefixes=None):
        suite, method = test_id.split(".", 1)
        key = f"{suite.rsplit('/', 1)[-1]}.{method.split('/', 1)[0]}"
        fragments = self.tests.get(key, [])
        if not fragments:
            raise SummaryError(f"{test_id}: テストコードを抽出できません")
        reached = []
        expanded = "\n".join(self.expand(f, [], "1", reached) for f in fragments)
        try:
            summary = "" if code_only else render_summary(expanded, parameterized or any(f.parameterized for f in fragments), param_count, partial, test_id)
        except SummaryError as error:
            raise SummaryError(f"{error}\nテスト定義元: {fragments[0].location}") from error
        code = "\n".join(dedent_source(f.source, f.indent) for f in fragments)
        for prefix, registered_suite, source in self.instantiations:
            if registered_suite == key.split(".", 1)[0] and (prefixes is None or prefix in prefixes):
                code += "\n" + source
        for name in ([] if code_only else reached):
            definition = self.definitions[name]
            code += f"\n// サブ手順: {name}\n// 定義元: {definition.location}\n" + dedent_source(definition.source, definition.indent)
        return summary + code


def discover(language, root="."):
    suffixes = {".cc", ".cpp"} if language == "c_cpp" else {".cs"}
    return sorted(p for p in Path(root).rglob("*") if p.is_file() and p.suffix in suffixes and not any(part in {"bin", "obj", "results", "gen", "gtest"} for part in p.parts))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--test-id")
    parser.add_argument("--list-inputs", action="store_true")
    parser.add_argument("--language", choices=["c_cpp", "dotnet"], default="c_cpp")
    parser.add_argument("--source", action="append", default=[])
    parser.add_argument("--is-windows", choices=["0", "1"])
    parser.add_argument("--encoding", default="utf-8")
    parser.add_argument("--parameterized", action="store_true")
    parser.add_argument("--param-count", type=int)
    parser.add_argument("--partial", action="store_true")
    parser.add_argument("--code-only", action="store_true")
    args = parser.parse_args()
    try:
        encoding = text_encoding(args.encoding)
        if hasattr(sys.stdout, "reconfigure"):
            sys.stdout.reconfigure(encoding="utf-8" if encoding == "utf-8-sig" else encoding)
            sys.stderr.reconfigure(encoding="utf-8")
        paths = list(discover(args.language)) + [Path(p) for p in args.source]
        index = SourceIndex.from_paths(paths, args.language, None if args.is_windows is None else args.is_windows == "1", encoding)
        if args.list_inputs:
            report = "".join(str(path) + "\n" for path in sorted(index.files))
        else:
            if not args.test_id:
                parser.error("--test-id が必要です")
            report = index.report(args.test_id, args.parameterized, args.param_count, args.partial, args.code_only)
    except (SummaryError, OSError, ValueError, LookupError) as error:
        print(f"[  FAILED  ] {error}", file=sys.stderr)
        return 1
    sys.stdout.write(report)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
