#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gcovr JSON を TEST_SRCS の正規パスへ変換する。"""

import argparse
import json
import re
import sys
from pathlib import Path

# 行末コメントの契約。分数は省略可 (判定トークンの無い行だけ)。
# 例: /* TESTFW_EXCL_EH_ARCS: 1/2 */ または // TESTFW_EXCL_EH_ARCS
MARKER_RE = re.compile(r"TESTFW_EXCL_EH_ARCS(?:\s*:\s*(\d+)\s*/\s*(\d+))?")
COMMENT_RE = re.compile(r"/\*.*?\*/|//.*")
# `::` のコロンは三項演算子とみなさない。
DECISION_RE = re.compile(r"(?<!:)\?(?!:)|\b(?:if|switch)\b|&&|\|\|")


def build_source_map(workspace_dir, test_sources):
    """TEST_SRCS のベース名からワークスペース相対パスへの対応を作る。"""
    # Windows では短縮パス (8.3 形式) と長いパスが混在しうるため、
    # test_sources 側の resolve() と表記を揃えるために workspace_dir も解決する。
    workspace_dir = Path(workspace_dir).resolve()
    source_map = {}
    duplicate_names = set()

    for source_text in test_sources:
        source = Path(source_text).resolve()
        try:
            relative = source.relative_to(workspace_dir).as_posix()
        except ValueError as error:
            raise ValueError(f"TEST_SRCS is outside the workspace: {source}") from error

        if source.name in source_map and source_map[source.name] != relative:
            duplicate_names.add(source.name)
        source_map[source.name] = relative

    if duplicate_names:
        names = ", ".join(sorted(duplicate_names))
        raise ValueError(f"TEST_SRCS contains duplicate basenames: {names}")

    return source_map


def normalize_coverage(data, source_map):
    """対象ソース以外を除外し、対象ソースを正規パスへ変換する。"""
    normalized_files = []
    seen_sources = set()

    for file_data in data.get("files", []):
        source_name = Path(file_data.get("file", "")).name
        canonical_name = source_map.get(source_name)
        if canonical_name is None:
            continue
        if canonical_name in seen_sources:
            raise ValueError(f"Coverage contains duplicate source entries: {canonical_name}")

        normalized_file = dict(file_data)
        normalized_file["file"] = canonical_name
        normalized_files.append(normalized_file)
        seen_sources.add(canonical_name)

    normalized = dict(data)
    normalized["files"] = normalized_files
    return normalized


def strip_comments(line):
    """行からコメントを除き、判定トークンの有無を見るための本文を返す。"""
    return COMMENT_RE.sub("", line)


def parse_eh_arc_markers(source_text):
    """ソースから TESTFW_EXCL_EH_ARCS を読む。値は (未到達, 総数) または None。"""
    markers = {}
    for index, raw_line in enumerate(source_text.splitlines(), start=1):
        match = MARKER_RE.search(raw_line)
        if match is None:
            continue
        if match.group(1) is None:
            markers[index] = None
        else:
            markers[index] = (int(match.group(1)), int(match.group(2)))
    return markers


def line_has_decision(raw_line):
    """コメントを除いた行にソース上の判定があるか。"""
    return DECISION_RE.search(strip_comments(raw_line)) is not None


def _branch_counts(line_data):
    branches = line_data.get("branches") or []
    uncovered = sum(1 for branch in branches if branch.get("count", 0) == 0)
    return uncovered, len(branches), branches


def apply_eh_arc_exclusions(data, workspace_dir, source_map):
    """マーカー行の未印 EH 弧を母数から外す。契約不一致は ValueError。"""
    workspace_dir = Path(workspace_dir)
    errors = []
    path_by_name = {Path(path).name: path for path in source_map.values()}

    for file_data in data.get("files", []):
        source_name = Path(file_data.get("file", "")).name
        relative = path_by_name.get(source_name)
        if relative is None:
            continue
        source_path = workspace_dir / relative
        if not source_path.is_file():
            errors.append(f"{relative}: source file is missing")
            continue
        source_text = source_path.read_text(encoding="utf-8")
        markers = parse_eh_arc_markers(source_text)
        if not markers:
            continue
        source_lines = source_text.splitlines()
        lines_by_number = {
            line_data.get("line_number"): line_data for line_data in file_data.get("lines") or []
        }

        for marker_line, contract in markers.items():
            raw_line = ""
            if 1 <= marker_line <= len(source_lines):
                raw_line = source_lines[marker_line - 1]
            location = f"{relative}:{marker_line}"
            # clang-format が継続行へコメントを移しても、枝は文先頭行に残る。
            # 判定行へ横滑りすると、未通過の本番条件を EH 弧と誤認して消す。
            candidate_numbers = [marker_line, marker_line - 1, marker_line + 1, marker_line + 2]
            target = None
            for candidate in candidate_numbers:
                line_data = lines_by_number.get(candidate)
                if line_data is None:
                    continue
                candidate_text = ""
                if 1 <= candidate <= len(source_lines):
                    candidate_text = source_lines[candidate - 1]
                if candidate != marker_line and line_has_decision(candidate_text):
                    continue
                uncovered, total, _branches = _branch_counts(line_data)
                if uncovered > 0:
                    target = (line_data, uncovered, total)
                    break
            if target is None:
                continue
            line_data, uncovered, total = target
            _unused, _unused_total, branches = _branch_counts(line_data)

            if contract is None:
                if line_has_decision(raw_line):
                    errors.append(
                        f"{location}: TESTFW_EXCL_EH_ARCS on a decision line requires "
                        f"uncovered/total (found {uncovered}/{total})"
                    )
                    continue
                line_data["branches"] = [
                    branch for branch in branches if branch.get("count", 0) > 0
                ]
                continue

            expected_uncovered, expected_total = contract
            if (uncovered, total) != (expected_uncovered, expected_total):
                errors.append(
                    f"{location}: TESTFW_EXCL_EH_ARCS expected {expected_uncovered}/"
                    f"{expected_total}, found {uncovered}/{total}"
                )
                continue
            line_data["branches"] = [
                branch for branch in branches if branch.get("count", 0) > 0
            ]

    if errors:
        raise ValueError("\n".join(errors))
    return data


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input_json", type=Path)
    parser.add_argument("output_json", type=Path)
    parser.add_argument("workspace_dir", type=Path)
    parser.add_argument("test_sources", nargs="+")
    return parser.parse_args()


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
    args = parse_args()

    workspace_dir = args.workspace_dir.resolve()
    with args.input_json.open(encoding="utf-8") as input_file:
        data = json.load(input_file)

    try:
        source_map = build_source_map(workspace_dir, args.test_sources)
        normalized = normalize_coverage(data, source_map)
        apply_eh_arc_exclusions(normalized, workspace_dir, source_map)
    except ValueError as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1

    if not normalized["files"] and data.get("files"):
        # TEST_SRCS がカバレッジ結果と 1 件も対応しない状態は、指定の誤りか、
        # 呼び出し側での単語分割の失敗を示す。空のまま進めるとカバレッジが
        # 無言で 0% になるため、標準エラーへ通知する。
        print(
            "Warning: no TEST_SRCS matched the coverage data: "
            + ", ".join(sorted(source_map)),
            file=sys.stderr,
        )

    with args.output_json.open("w", encoding="utf-8", newline="\n") as output_file:
        json.dump(normalized, output_file, ensure_ascii=False, separators=(",", ":"))
        output_file.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
