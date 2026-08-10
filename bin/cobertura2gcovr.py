#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cobertura2gcovr.py - Cobertura XML を gcovr 形式のテキストレポートに変換するスクリプト

使用方法:
    python cobertura2gcovr.py <cobertura.xml>

引数:
    cobertura.xml  - 入力となる Cobertura 形式の XML ファイル

出力:
    標準出力に gcovr 形式のカバレッジレポートを出力する。
"""

import sys
import os
import re
import xml.etree.ElementTree as ET


def parse_condition_coverage(coverage_str):
    """
    condition-coverage 文字列からカバー数と総数を抽出する。

    Args:
        coverage_str: "50% (1/2)" 形式の文字列

    Returns:
        (covered, valid) のタプル、パース失敗時は (0, 0)
    """
    if not coverage_str:
        return (0, 0)
    match = re.search(r'\((\d+)/(\d+)\)', coverage_str)
    if match:
        return (int(match.group(1)), int(match.group(2)))
    return (0, 0)


def parse_cobertura(xml_path):
    """
    Cobertura XML をパースし、ファイルごとのカバレッジ情報を抽出する。

    Args:
        xml_path: Cobertura XML ファイルのパス

    Returns:
        list: [(ファイル名, 総行数, 実行行数, 未実行行番号リスト, 総分岐数, 実行分岐数)] のリスト
    """
    tree = ET.parse(xml_path)
    root = tree.getroot()

    coverage_data = []

    for package in root.findall('.//package'):
        for cls in package.findall('.//class'):
            filename = cls.get('filename')
            # ファイル名のみを取得
            basename = os.path.basename(filename)

            total_lines = 0
            exec_lines = 0
            missing_lines = []
            total_branches = 0
            exec_branches = 0

            for line in cls.findall('./lines/line'):
                line_num = int(line.get('number'))
                hits = int(line.get('hits'))
                total_lines += 1
                if hits > 0:
                    exec_lines += 1
                else:
                    missing_lines.append(line_num)

                # 分岐カバレッジを抽出
                if line.get('branch') == 'true':
                    cov = parse_condition_coverage(line.get('condition-coverage'))
                    total_branches += cov[1]
                    exec_branches += cov[0]

            coverage_data.append((basename, total_lines, exec_lines, missing_lines,
                                total_branches, exec_branches))

    return coverage_data


def format_missing(missing_lines):
    """
    未実行行番号リストを範囲形式の文字列に変換する。

    Args:
        missing_lines: 未実行行番号のリスト

    Returns:
        str: "1-3, 5, 7-9" のような形式の文字列
    """
    if not missing_lines:
        return ""

    missing_lines = sorted(missing_lines)
    ranges = []
    start = missing_lines[0]
    end = start

    for line in missing_lines[1:]:
        if line == end + 1:
            end = line
        else:
            if start == end:
                ranges.append(str(start))
            else:
                ranges.append(f"{start}-{end}")
            start = line
            end = line

    # 最後の範囲を追加
    if start == end:
        ranges.append(str(start))
    else:
        ranges.append(f"{start}-{end}")

    return ", ".join(ranges)


def format_rate(covered, total):
    """
    網羅率を百分率の文字列に整形する。

    Args:
        covered: 網羅した件数
        total: 総件数

    Returns:
        "100%" のような文字列。総件数が 0 の場合は "0%"
    """
    if total <= 0:
        return "0%"

    return f"{(covered / total) * 100:.0f}%"


def print_report(coverage_data):
    """
    gcovr 形式のレポートを出力する。

    Args:
        coverage_data: [(ファイル名, 総行数, 実行行数, 未実行行番号リスト, 総分岐数, 実行分岐数)] のリスト
    """
    separator = "-" * 78
    header = "Code Coverage Report"

    print(separator)
    print(f"{header:^78}")
    print(separator)
    print(f"{'File':<32} {'Lines':>6} {'Exec':>6} {'Cover':>6} "
          f"{'Branch':>7} {'BrCov':>6}   Missing")
    print(separator)

    total_lines = 0
    total_exec = 0
    total_branches = 0
    total_branches_exec = 0

    for data in coverage_data:
        filename, lines, exec_lines, missing = data[0], data[1], data[2], data[3]
        branches, branches_exec = data[4], data[5]

        total_lines += lines
        total_exec += exec_lines
        total_branches += branches
        total_branches_exec += branches_exec

        cover_str = format_rate(exec_lines, lines)
        # 分岐情報を持たない計測 (OpenCppCoverage は行カバレッジのみ) は 0% と区別して "-" を表示する
        # see: https://github.com/OpenCppCoverage/OpenCppCoverage#features
        branch_str = str(branches) if branches > 0 else "-"
        br_cover_str = format_rate(branches_exec, branches) if branches > 0 else "-"

        missing_str = format_missing(missing)

        # ファイル名が長い場合は切り詰め
        if len(filename) > 32:
            filename = filename[:29] + "..."

        row = (f"{filename:<32} {lines:>6} {exec_lines:>6} {cover_str:>6} "
               f"{branch_str:>7} {br_cover_str:>6}")
        if missing_str:
            print(f"{row}   {missing_str}")
        else:
            print(row)

    print(separator)

    # 合計行
    total_cover_str = format_rate(total_exec, total_lines)
    total_branch_str = str(total_branches) if total_branches > 0 else "-"
    total_br_cover_str = (format_rate(total_branches_exec, total_branches)
                          if total_branches > 0 else "-")

    print(f"{'TOTAL':<32} {total_lines:>6} {total_exec:>6} {total_cover_str:>6} "
          f"{total_branch_str:>7} {total_br_cover_str:>6}")
    print(separator)


def main():
    # 標準出力を UTF-8 に設定 (Windows 対応)
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        # Python 3.7 未満では reconfigure が存在しない
        pass

    if len(sys.argv) != 2:
        print("Usage: python cobertura2gcovr.py <cobertura.xml>",
              file=sys.stderr)
        sys.exit(1)

    xml_path = sys.argv[1]

    # 入力ファイルの存在確認
    if not os.path.exists(xml_path):
        print(f"Error: Input file not found: {xml_path}", file=sys.stderr)
        sys.exit(1)

    # Cobertura XML をパース
    try:
        coverage_data = parse_cobertura(xml_path)
    except ET.ParseError as e:
        print(f"Error: Failed to parse XML: {e}", file=sys.stderr)
        sys.exit(1)

    # レポートを出力
    print_report(coverage_data)


if __name__ == '__main__':
    main()
