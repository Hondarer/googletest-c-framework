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
import xml.etree.ElementTree as ET


def parse_cobertura(xml_path):
    """
    Cobertura XML をパースし、ファイルごとのカバレッジ情報を抽出する。

    Args:
        xml_path: Cobertura XML ファイルのパス

    Returns:
        list: [(ファイル名, 総行数, 実行行数, 未実行行番号リスト)] のリスト
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

            for line in cls.findall('.//line'):
                line_num = int(line.get('number'))
                hits = int(line.get('hits'))
                total_lines += 1
                if hits > 0:
                    exec_lines += 1
                else:
                    missing_lines.append(line_num)

            coverage_data.append((basename, total_lines, exec_lines, missing_lines))

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


def print_report(coverage_data):
    """
    gcovr 形式のレポートを出力する。

    Args:
        coverage_data: [(ファイル名, 総行数, 実行行数, 未実行行番号リスト)] のリスト
    """
    separator = "-" * 78
    header = "OpenCppCoverage Code Coverage Report"

    print(separator)
    print(f"{header:^78}")
    print(separator)
    print(f"{'File':<40} {'Lines':>7} {'Exec':>7} {'Cover':>6}   Missing")
    print(separator)

    total_lines = 0
    total_exec = 0

    for filename, lines, exec_lines, missing in coverage_data:
        total_lines += lines
        total_exec += exec_lines

        if lines > 0:
            cover = (exec_lines / lines) * 100
            cover_str = f"{cover:.0f}%"
        else:
            cover_str = "0%"

        missing_str = format_missing(missing)

        # ファイル名が長い場合は切り詰め
        if len(filename) > 40:
            filename = filename[:37] + "..."

        if missing_str:
            print(f"{filename:<40} {lines:>7} {exec_lines:>7} {cover_str:>6}   {missing_str}")
        else:
            print(f"{filename:<40} {lines:>7} {exec_lines:>7} {cover_str:>6}")

    print(separator)

    # 合計行
    if total_lines > 0:
        total_cover = (total_exec / total_lines) * 100
        total_cover_str = f"{total_cover:.0f}%"
    else:
        total_cover_str = "0%"

    print(f"{'TOTAL':<40} {total_lines:>7} {total_exec:>7} {total_cover_str:>6}")
    print(separator)


def main():
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
