#!/usr/bin/env python3
"""テスト結果の Markdown 表示と UTF-8 ファイル出力を共通化する。"""

import argparse
import json
import os
from pathlib import Path
import re
import sys
from urllib.parse import quote

from test_summary import text_encoding

ANSI = re.compile(r"\x1b(?:\[[0-?]*[ -/]*[@-~]|\][^\x07]*(?:\x07|\x1b\\))")


def clean(text):
    return ANSI.sub("", text).replace("\r\n", "\n").replace("\r", "\n")


def fence(text, language="text", fixed=False):
    lines = text.split("\n")
    while lines and not lines[-1].strip():
        lines.pop()
    text = "\n".join(lines)
    if not text.strip():
        return "なし"
    length = 3 if fixed else max(3, max((len(m.group()) + 1 for m in re.finditer(r"`+", text)), default=0))
    marker = "`" * length
    return f"{marker}{language}\n{text}\n{marker}"


def inline(text):
    text = str(text).replace("\n", " ")
    length = max((len(m.group()) + 1 for m in re.finditer(r"`+", text)), default=1)
    marker = "`" * length
    pad = " " if "`" in text or text.startswith(" ") or text.endswith(" ") else ""
    return marker + pad + text + pad + marker


def cell(text):
    return str(text).replace("|", r"\|").replace("\n", " ")


def link(label, path):
    return f"[{label}]({quote(str(path).replace(chr(92), '/'), safe='/')})"


def table(headers, rows, numeric=False):
    separator = ["---"] + (["---:"] * (len(headers) - 1) if numeric else ["---"] * (len(headers) - 1))
    return "\n".join("| " + " | ".join(cell(v) for v in row) + " |" for row in [headers, separator, *rows])


def admonition(kind, message):
    lines = message.strip().splitlines()
    body = fence(message) if len(lines) > 1 else (lines[0] if lines else "なし")
    return f"> [!{kind}]\n" + "\n".join("> " + line if line else ">" for line in body.splitlines())


def read(path, encoding="utf-8", replace=False):
    return Path(path).read_text(encoding=text_encoding(encoding), errors="replace" if replace else "strict")


def write(path, text):
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    Path(path).write_text(text.rstrip("\n") + "\n", encoding="utf-8", newline="\n")


def individual(test_id, status, evidence, output, binary="", comment="", definition="", error="", console=False):
    parts = ["# " + inline(test_id)]
    metadata = [] if console else [f"- 判定: {status}"]
    if binary:
        metadata.append("- テスト バイナリ: " + inline(binary))
    if comment:
        metadata.append("- 備考: " + inline(comment))
    if metadata:
        parts.append("\n".join(metadata))
    if error:
        _, _, code = evidence.partition("## テスト コード\n")
        evidence = "## テスト項目\n\n" + admonition("CAUTION", error) + "\n\n## テスト コード\n" + (code or "\nなし")
    if definition and not error:
        if definition == "auto":
            from gtest_summary_groups import definition_id
            definition = os.path.relpath(Path(definition_id(test_id)) / "results.md", Path(test_id)).replace("\\", "/")
        _, _, code = evidence.partition("## テスト コード\n")
        evidence = "## テスト項目\n\nテスト項目は " + link("テスト定義の結果", definition) + " を参照してください。\n\n## テスト コード\n" + (code or "\nなし")
    parts.append(evidence.strip() or "## テスト項目\n\nなし\n\n## テスト コード\n\nなし")
    parts.append("## 実行結果\n\n" + fence(output if console else clean(output), fixed=console))
    return "\n\n".join(parts) + "\n"


def execution_output(content):
    """コード内の見出しを無視し、実行結果の本文だけを取り出す。"""
    marker = None
    lines = content.split("\n")
    for i, line in enumerate(lines):
        match = re.fullmatch(r"(`{3,})(\w*)", line)
        if match:
            if marker is None:
                marker = match[1]
            elif match[1] == marker and not match[2]:
                marker = None
        elif marker is None and line == "## 実行結果":
            block = "\n".join(lines[i + 1:]).strip("\n")
            if block == "なし":
                return ""
            return "\n".join(block.split("\n")[1:-1])
    raise ValueError("実行結果の節がありません")


def definition(test_id, evidence, records, total):
    counts = {status: sum(r["status"] == status for r in records) for status in ("PASSED", "WARNING", "FAILED")}
    status = "FAILED" if counts["FAILED"] else "WARNING" if counts["WARNING"] else "PASSED"
    n = str(len(records)) + (f" / {total}" if len(records) != total else "")
    distribution = ", ".join(f"{key}: {value}" for key, value in counts.items() if value)
    parts = ["# " + inline(test_id), f"- 総合判定: {status}\n- 実行レコード数: {n} ({distribution})", evidence.strip(), "## 実行レコード"]
    for record in records:
        parts += ["### " + inline(record["name"]), f'- 判定: {record["status"]}\n- 詳細: ' + link("results.md", record["path"]), fence(record["output"])]
    return "\n\n".join(parts) + "\n"


def coverage_rows(data):
    from cobertura2gcovr import format_missing, format_rate
    rows = []
    for name, lines, executed, missing, branches, covered in data:
        name = name[:29] + "..." if len(name) > 32 else name
        rows.append([name, lines, executed, format_rate(executed, lines), branches or "-", format_rate(covered, branches) if branches else "-", format_missing(missing)])
    lines = sum(d[1] for d in data)
    executed = sum(d[2] for d in data)
    branches = sum(d[4] for d in data)
    covered = sum(d[5] for d in data)
    rows.append(["TOTAL", lines, executed, format_rate(executed, lines), branches or "-", format_rate(covered, branches) if branches else "-", ""])
    return table(["File", "Lines", "Exec", "Cover", "Branch", "BrCov", "Missing"], rows)


def summary(directory, journal, counts_dir, coverage="", filter_value=None, journal_dir="."):
    # UTF-8 の診断は、ソース文字コードのジャーナルに混ぜず参照する。
    journal = "\n".join(read(Path(journal_dir) / line.split("\t", 1)[1]).rstrip("\n")
                        if line.startswith("@utf8\t") else line
                        for line in journal.splitlines())
    text = clean(journal)
    start = re.search(r"^Test start on (.*)\.$", text, re.M)
    parts = ["# " + inline(directory) + " のテスト結果サマリー"]
    if start:
        parts.append("- 開始日時: " + start[1])
    if filter_value is not None:
        parts.append(admonition("NOTE", "GTEST_FILTER = " + inline(filter_value)))
    md5 = re.findall(r"^([0-9a-f]{32})  (.+)$", text, re.M)
    if md5:
        parts.append("## テスト対象ソースの MD5\n\n" + table(["MD5", "ファイル"], [(h, inline(p)) for h, p in md5]))
    results = []
    definitions = []
    from gtest_summary_groups import definition_id
    for line in text.splitlines():
        columns = line.split("\t")
        if len(columns) >= 2 and columns[1] in {"PASSED", "WARNING", "FAILED"}:
            name, status = columns[:2]
            results.append([link(inline(name), "../" + name + "/results.md"), status, inline(columns[2]) if len(columns) > 2 and columns[2] else ""])
            key = definition_id(name)
            if key not in definitions:
                definitions.append(key)
    parts.append("## テスト結果\n\n" + (table(["テスト ID", "結果", "備考"], results) if results else "なし"))
    aggregation = []
    for label in ("Total tests", "Passed", "Warning(s)", "Failed"):
        match = re.search(r"^" + re.escape(label) + r"\t+(.*)$", text, re.M)
        aggregation.append([label, match[1] if match else str(len(results)) if label == "Total tests" else "0"])
    parts.append("## 集計\n\n" + table(["項目", "件数"], aggregation))
    rows, normal, abnormal, unknown = [], 0, 0, 0
    for key in definitions:
        path = Path(counts_dir) / (key + ".json")
        count = json.loads(read(path)) if path.exists() else None
        if count is None:
            values = ["未評価"] * 3
            unknown += 1
        else:
            a, b = count["正常系"], count["異常系"]
            normal += a
            abnormal += b
            values = [a, b, a + b]
        rows.append([link(inline(key), "../" + key + "/results.md"), *values])
    if rows:
        rows.append(["合計", normal, abnormal, normal + abnormal])
    checks = table(["テスト定義", "正常系", "異常系", "計"], rows, True) if rows else "なし"
    if unknown:
        checks += f"\n\n合計には、未評価のテスト定義 {unknown} 件を含みません。"
    parts.append("## 確認件数\n\n" + checks)
    diagnostics = []
    context = ""
    lines = text.splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith("@test\t"):
            context = line.split("\t", 1)[1]
        if re.search(r"\[ *WARNING *\]|\[ *FAILED *\]|^Error:", line):
            message = (inline(context) + ": " if context else "") + line
            details = []
            i += 1
            while i < len(lines) and lines[i] and not re.search(r"\t|^----|^@|\[ *WARNING *\]|\[ *FAILED *\]|^Error:", lines[i]):
                details.append(lines[i])
                i += 1
            if details:
                message += "\n" + "\n".join(details)
            diagnostics.append(admonition("WARNING" if "WARNING" in line else "CAUTION", message))
            continue
        i += 1
    if diagnostics:
        parts.append("## 警告とエラー\n\n" + "\n\n".join(diagnostics))
    if coverage and Path(coverage).exists():
        parts.append("## カバレッジ\n\n" + read(coverage).strip())
    return "\n\n".join(parts) + "\n"


def gcov(body, workspace):
    body = body.replace("\r\n", "\n").replace("\r", "\n")
    match = re.search(r"^.*?0:Source:(.*)$", body, re.M)
    source = match[1] if match else ""
    normalized = source.replace("\\", "/")
    root = str(Path(workspace).resolve()).replace("\\", "/").rstrip("/")
    within_workspace = (normalized.casefold().startswith((root + "/").casefold())
                        if sys.platform == "win32" else normalized.startswith(root + "/"))
    if within_workspace:
        source = normalized[len(root) + 1:]
    elif source:
        try:
            source = Path(source).resolve().relative_to(Path(workspace).resolve()).as_posix()
        except ValueError:
            pass
    name = normalized.rsplit("/", 1)[-1]
    return "# " + inline(name) + " のカバレッジ\n\n- ソース: " + inline(source) + "\n\n" + fence(body) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=["individual", "definition", "summary", "gcov", "decode"])
    parser.add_argument("--output")
    parser.add_argument("--console-output")
    parser.add_argument("--dotnet-color", action="store_true")
    parser.add_argument("--input")
    parser.add_argument("--encoding", default="utf-8")
    parser.add_argument("--test-id", default="")
    parser.add_argument("--status", default="PASSED")
    parser.add_argument("--binary", default="")
    parser.add_argument("--comment", default="")
    parser.add_argument("--evidence")
    parser.add_argument("--error")
    parser.add_argument("--definition", default="")
    parser.add_argument("--console", action="store_true")
    parser.add_argument("--console-encoding")
    parser.add_argument("--prefix", action="store_true")
    parser.add_argument("--counts-dir", default="")
    parser.add_argument("--directory", default="")
    parser.add_argument("--coverage", default="")
    parser.add_argument("--filter")
    parser.add_argument("--workspace", default="")
    args = parser.parse_args()
    encoding = text_encoding(args.encoding)
    console_encoding = text_encoding(args.console_encoding) if args.console_encoding else encoding
    if console_encoding == "utf-8-sig":
        console_encoding = "utf-8"
    sys.stdout.reconfigure(encoding=console_encoding if args.console else "utf-8", newline="\n")
    sys.stderr.reconfigure(encoding="utf-8")
    if args.mode == "individual":
        evidence = read(args.evidence) if args.evidence else ""
        error = read(args.error) if args.error else ""
        output = read(args.input, encoding, True) if args.input else ""
        comment = os.fsencode(args.comment).decode(encoding, errors="replace") if encoding not in ("utf-8", "utf-8-sig") else args.comment
        content = individual(args.test_id, args.status, evidence, output, args.binary, comment, args.definition, error, args.console)
        if args.console_output:
            console = individual(args.test_id, args.status, evidence, output, args.binary, comment, args.definition, error, True)
            if args.dotnet_color:
                for line, color in (("テストの実行に成功しました。", 32), ("Test Run Successful.", 32),
                                    ("テストの実行に失敗しました。", 31), ("Test Run Failed.", 31)):
                    console = re.sub("^" + re.escape(line) + "$", lambda m: f"\x1b[{color}m{m[0]}\x1b[0m", console, flags=re.M)
            Path(args.console_output).write_text(console, encoding=console_encoding, newline="\n")
        if args.prefix:
            content = content.split("## 実行結果\n", 1)[0] + "## 実行結果\n\n```text\n"
    elif args.mode == "decode":
        content = read(args.input, encoding, True) if args.input else sys.stdin.buffer.read().decode(encoding, errors="replace")
    elif args.mode == "summary":
        content = summary(args.directory, read(args.input, encoding, True), args.counts_dir, args.coverage, args.filter, Path(args.input).parent)
    elif args.mode == "definition":
        state = json.loads(read(args.input))
        content = definition(**state)
    else:
        source = Path(args.input)
        if source.is_dir():
            for path in sorted(source.glob("*.gcov")):
                write(Path(args.output) / (path.name + ".md"), gcov(read(path, encoding, True), args.workspace))
            return
        content = gcov(read(args.input, encoding, True), args.workspace)
    if args.output:
        write(args.output, content)
    else:
        sys.stdout.write(content)


if __name__ == "__main__":
    main()
