#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gcovr JSON を TEST_SRCS の正規パスへ変換する。"""

import argparse
import json
import sys
from pathlib import Path


def build_source_map(workspace_dir, test_sources):
    """TEST_SRCS のベース名からワークスペース相対パスへの対応を作る。"""
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
    except ValueError as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1

    with args.output_json.open("w", encoding="utf-8", newline="\n") as output_file:
        json.dump(normalized, output_file, ensure_ascii=False, separators=(",", ":"))
        output_file.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
