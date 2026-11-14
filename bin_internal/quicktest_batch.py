#!/usr/bin/env python3
"""Google Test の一括実行結果と標準出力から、個別テストの Markdown エビデンスを一括生成する。"""

import argparse
import json
import os
from pathlib import Path
import re
import sys

# 埋め込み版 Python (._pth) はスクリプトのディレクトリを sys.path に入れないため、明示的に追加する。
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

sys.stdout.reconfigure(encoding="utf-8")
sys.stderr.reconfigure(encoding="utf-8")

from add_gtest_color import colorize_line, enable_windows_ansi
from results_markdown import PARAMETER_BYTES, PARAMETER_WARNING, RUN_MODE_BATCH, individual, write
from test_subprocedures import SourceIndex, discover
from test_summary import SummaryError, text_encoding

# Google Test の見出しは "[ RUN      ]"、"[       OK ]"、"[  SKIPPED ]"、"[  FAILED  ]" の表記で出力される。
# テストが改行なしで出力を終えると、見出しは同じ行の途中から始まるため、行頭には固定しない。
# 失敗したパラメーター テストの終了行は、テスト名の後に ", where GetParam() = ..." が続く。
# see: https://github.com/google/googletest/blob/v1.17.0/googletest/src/gtest.cc
RUN_LINE = re.compile(r"\[ RUN      \] (\S+)\s*$")
END_LINE = re.compile(r"\[(       OK |  SKIPPED |  FAILED  )\] (\S+?)(?:, where .*?)?(?: \((\d+) ms\))?\s*$")
DIAG_LINE = re.compile(r"\[ *WARNING *\]|\[ *FAILED *\]|^Error:")
WARNING_LINE = re.compile(r"\[ *WARNING *\]")

# テストごとの結果。OK・SKIPPED・FAILED は Google Test の終了行に対応する。
OUTCOME_OK = "OK"
OUTCOME_SKIPPED = "SKIPPED"
OUTCOME_FAILED = "FAILED"
# DISABLED_ により実行されなかったテスト。個別実行でも 0 件の合格として扱われる。
OUTCOME_DISABLED = "DISABLED"
# 終了行を出力する前にテスト バイナリが終了したテスト。
OUTCOME_ABORTED = "ABORTED"
# テスト バイナリが途中で終了したため、実行されなかったテスト。
OUTCOME_NOT_RUN = "NOT_RUN"

FAILED_OUTCOMES = (OUTCOME_FAILED, OUTCOME_ABORTED, OUTCOME_NOT_RUN)


def parse_selected_list(text):
    items = []
    for raw in text.splitlines():
        line = raw.rstrip("\r\n")
        if not line:
            continue
        comment = "#" + line.split("#", 1)[1] if "#" in line else ""
        name = line.split(" ", 1)[0]
        parts = name.split("/")
        test_id = f"{parts[1]}/{parts[0]}/{parts[2]}" if len(parts) == 3 else name
        items.append((name, test_id, comment))
    return items


def parse_gtest_json(path, fallback_encoding):
    raw = Path(path).read_bytes()
    try:
        text = raw.decode("utf-8")
    except UnicodeDecodeError:
        text = raw.decode(fallback_encoding, errors="replace")
    data = json.loads(text)
    cases = {}
    for suite in data.get("testsuites", []):
        suite_name = suite.get("name", "")
        for case in suite.get("testsuite", []):
            case_name = case.get("name", "")
            cases[f"{suite_name}.{case_name}"] = case
    return cases


def load_json_cases(path, fallback_encoding):
    """JSON を読み込む。テスト バイナリが異常終了した場合は生成されないため、空として扱う。"""
    try:
        return parse_gtest_json(path, fallback_encoding)
    except (OSError, ValueError):
        return {}


def parse_batch_stdout(text):
    """標準出力をテストごとの区間へ分ける。

    戻り値は (main() の案内行, {テスト名: (終了種別, 所要時間, 本文)}, テスト区間外の診断行)。
    終了行のない区間は、終了種別を None とする。
    """
    main_line = ""
    blocks = {}
    outside = []
    current_name = None
    current_lines = []

    def close(kind, elapsed):
        blocks[current_name] = (kind, elapsed, "".join(current_lines))

    for raw in text.splitlines():
        line = raw.rstrip("\r\n")
        if line.startswith("Note: Google Test filter") or line.startswith("Your program stop with error code:"):
            continue
        run_match = RUN_LINE.search(line)
        if current_name is not None:
            if run_match and run_match.group(1) != current_name:
                # 終了行を識別できないまま次のテストが始まった。区間だけを確定し、判定は JSON に委ねる。
                close(None, "0")
                current_name = None
            else:
                current_lines.append(line + "\n")
                match = END_LINE.search(line)
                if match and match.group(2) == current_name:
                    close(match.group(1).strip(), match.group(3) or "0")
                    current_name = None
                continue
        if run_match:
            current_name = run_match.group(1)
            current_lines = [line[run_match.start():] + "\n"]
        elif line.startswith("Running main() from ") and not main_line and not blocks:
            main_line = line
        elif DIAG_LINE.search(line):
            outside.append(line)
    if current_name is not None:
        close(None, "0")
    return main_line, blocks, outside


def is_disabled_name(name):
    return any(part.startswith("DISABLED_") for part in re.split(r"[./]", name))


def resolve_outcome(name, blocks, json_cases):
    """標準出力の区間と JSON から、テスト 1 件の結果を決める。"""
    case = json_cases.get(name) or {}
    if name in blocks:
        kind = blocks[name][0]
        if case.get("failures"):
            return OUTCOME_FAILED
        if kind is not None:
            return kind
        # 終了行を識別できなかった区間。JSON が完了を示す場合だけ、その結果を採用する。
        if case.get("status") == "RUN" and case.get("result") == "COMPLETED":
            return OUTCOME_OK
        if case.get("status") == "RUN" and case.get("result") == "SKIPPED":
            return OUTCOME_SKIPPED
        return OUTCOME_ABORTED
    if case:
        if case.get("status") == "NOTRUN" and case.get("result") == "SUPPRESSED":
            return OUTCOME_DISABLED
        return OUTCOME_NOT_RUN
    return OUTCOME_DISABLED if is_disabled_name(name) else OUTCOME_NOT_RUN


def build_runtime_text(binary, name, main_line, outcome, block=None, exit_code=0):
    """個別実行と同じ形の実行結果を組み立てる。"""
    command_line = f"./{binary} --gtest_filter={name}\n" + (f"{main_line}\n" if main_line else "")
    if outcome == OUTCOME_DISABLED:
        return (
            command_line
            + "[==========] Running 0 tests from 0 test suites.\n"
            + "[==========] 0 tests from 0 test suites ran. (0 ms total)\n"
            + "[  PASSED  ] 0 tests.\n"
            + "\n"
            + "  YOU HAVE 1 DISABLED TEST\n"
            + "\n"
        )
    if outcome == OUTCOME_NOT_RUN:
        return f"[  FAILED  ] Not executed: the batch run ended before this test (exit code: {exit_code}).\n"

    _, elapsed, block_text = block
    suite_name = name.split(".", 1)[0]
    header = (
        command_line
        + "[==========] Running 1 test from 1 test suite.\n"
        + "[----------] Global test environment set-up.\n"
        + f"[----------] 1 test from {suite_name}\n"
    )
    if outcome == OUTCOME_ABORTED:
        return header + block_text + f"\n[  FAILED  ] Terminated abnormally during the batch run (exit code: {exit_code}).\n"

    if outcome == OUTCOME_OK:
        result_lines = "[  PASSED  ] 1 test.\n"
    elif outcome == OUTCOME_SKIPPED:
        result_lines = f"[  PASSED  ] 0 tests.\n[  SKIPPED ] 1 test, listed below:\n[  SKIPPED ] {name}\n"
    else:
        result_lines = f"[  PASSED  ] 0 tests.\n[  FAILED  ] 1 test, listed below:\n[  FAILED  ] {name}\n\n 1 FAILED TEST\n"
    footer = (
        f"[----------] 1 test from {suite_name} ({elapsed} ms total)\n"
        + "\n"
        + "[----------] Global test environment tear-down\n"
        + f"[==========] 1 test from 1 test suite ran. ({elapsed} ms total)\n"
        + result_lines
    )
    return header + block_text + footer


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--selected-list", required=True)
    parser.add_argument("--json", required=True)
    parser.add_argument("--stdout", required=True)
    parser.add_argument("--binary", required=True)
    parser.add_argument("--executed-at", required=True)
    parser.add_argument("--exit-code", type=int, default=0)
    parser.add_argument("--is-windows", choices=["0", "1"], default="0")
    parser.add_argument("--encoding", default="utf-8")
    parser.add_argument("--report-state", required=True)
    args = parser.parse_args()

    encoding = text_encoding(args.encoding)
    output_encoding = "utf-8" if encoding == "utf-8-sig" else encoding
    selected = parse_selected_list(Path(args.selected_list).read_text(encoding=encoding, errors="replace"))
    json_cases = load_json_cases(args.json, encoding)
    main_line, blocks, outside = parse_batch_stdout(Path(args.stdout).read_text(encoding=encoding, errors="replace"))

    index = None
    index_error = None
    try:
        index = SourceIndex.from_paths(discover("c_cpp"), "c_cpp", args.is_windows == "1", encoding)
    except (SummaryError, OSError, ValueError, LookupError) as error:
        index_error = error

    # 出力とファイル生成の前に、全テスト分の内容を組み立てる。
    records = []
    for name, test_id, comment in selected:
        is_param = "/" in name.split(".", 1)[1]
        defn = "auto" if is_param else ""
        counts = {}
        evidence = ""
        evidence_error = ""
        try:
            if index is None:
                raise index_error
            evidence = index.report(name, code_only=is_param, counts=counts)
        except (SummaryError, OSError, ValueError, LookupError) as error:
            # 個別実行と同じく、エビデンスを生成できないテストは実行結果を採用せず失敗にする。
            evidence_error = f"[  FAILED  ] {error}\n[  FAILED  ] Test evidence generation failed.\n"

        if evidence_error:
            final_status = "FAILED"
            runtime_text = ""
            diagnostics = evidence_error.splitlines(keepends=True)
            console = "\n" + individual(test_id, "FAILED", "", "", args.binary, comment, defn, evidence_error, console=True)
            counts = None
        else:
            outcome = resolve_outcome(name, blocks, json_cases)
            runtime_text = build_runtime_text(args.binary, name, main_line, outcome, blocks.get(name), args.exit_code)
            if outcome in FAILED_OUTCOMES:
                final_status = "FAILED"
            else:
                if PARAMETER_BYTES.match(comment):
                    runtime_text += f"\x1b[33m[ WARNING ]\x1b[0m {PARAMETER_WARNING}\n"
                final_status = "WARNING" if WARNING_LINE.search(runtime_text) else "PASSED"
            diagnostics = [line + "\n" for line in runtime_text.splitlines() if DIAG_LINE.search(line)]
            console_doc = individual(test_id, "PASSED", evidence, "", args.binary, comment, defn, console=True)
            console_prefix = console_doc.split("## 実行結果\n", 1)[0] + "## 実行結果\n\n```text\n"
            colored_runtime = "".join(colorize_line(line) + "\n" for line in runtime_text.splitlines())
            console = "\n" + console_prefix + colored_runtime + "```\n"
            if is_param:
                counts = None

        records.append({
            "test_id": test_id,
            "comment": comment,
            "status": final_status,
            "counts": counts,
            "markdown": individual(
                test_id, final_status, evidence, runtime_text, args.binary, comment, defn, evidence_error,
                console=False, executed_at=args.executed_at, run_mode=RUN_MODE_BATCH,
            ),
            "console": console,
            "diagnostics": diagnostics,
        })

    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding=output_encoding, errors="surrogateescape", newline="\n")
    if args.is_windows == "1":
        enable_windows_ansi()

    report_state = Path(args.report_state)
    counts_dir = report_state / "counts"
    counts_dir.mkdir(parents=True, exist_ok=True)

    status_colors = {"PASSED": "32", "WARNING": "33", "FAILED": "31"}
    tally = {"PASSED": 0, "WARNING": 0, "FAILED": 0}
    journal_lines = []
    summary_lines = []

    for record in records:
        test_id = record["test_id"]
        comment = record["comment"]
        final_status = record["status"]
        if record["counts"] is not None:
            count_path = counts_dir / f"{test_id}.json"
            count_path.parent.mkdir(parents=True, exist_ok=True)
            count_path.write_text(json.dumps(record["counts"], ensure_ascii=False), encoding="utf-8")

        tally[final_status] += 1
        write(Path("results") / test_id / "results.md", record["markdown"])
        sys.stdout.write(record["console"])

        journal_lines.append(f"@test\t{test_id}\n")
        journal_lines.append(f"{test_id}\t{final_status}\t{comment}\n")
        journal_lines.extend(record["diagnostics"])
        summary_lines.append(f"{test_id}\t\x1b[{status_colors[final_status]}m{final_status}\x1b[0m\t{comment}\n")

    # テスト区間外の結果。個別のテストへ割り当てられないため、全体の件数として加える。
    extra_lines = []
    outside_warnings = [line for line in outside if WARNING_LINE.search(line)]
    if outside_warnings:
        tally["WARNING"] += 1
        extra_lines.extend(outside_warnings)
    if args.exit_code != 0 and tally["FAILED"] == 0:
        # SetUpTestSuite やグローバル環境の失敗など、どのテストの失敗にも現れない異常終了。
        tally["FAILED"] += 1
        extra_lines.extend(line for line in outside if not WARNING_LINE.search(line))
        extra_lines.append(f"[  FAILED  ] Test binary exited with code {args.exit_code} outside individual tests.")
    if extra_lines:
        journal_lines.append("@test\t\n")
        journal_lines.extend(line + "\n" for line in extra_lines)
        sys.stdout.write("\n" + "".join(colorize_line(line) + "\n" for line in extra_lines))

    with open(report_state / "summary", "a", encoding=output_encoding, errors="replace", newline="\n") as handle:
        handle.write("".join(journal_lines))
    (report_state / "batch_counts").write_text(
        f"{tally['PASSED']}\n{tally['WARNING']}\n{tally['FAILED']}\n", encoding="utf-8", newline="\n"
    )
    (report_state / "batch_summary").write_text(
        "".join(summary_lines), encoding=output_encoding, errors="replace", newline="\n"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
