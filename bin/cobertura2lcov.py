#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cobertura2lcov.py - Cobertura XML を lcov info 形式に変換するスクリプト

使用方法:
    python cobertura2lcov.py <cobertura.xml> [output.info]

引数:
    cobertura.xml  - 入力となる Cobertura 形式の XML ファイル
    output.info    - 出力ファイル (省略時: 標準出力)

出力:
    lcov info 形式のカバレッジ情報を出力する。
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
    Cobertura XML をパースし、ソースファイルごとのカバレッジ情報を抽出する。

    Args:
        xml_path: Cobertura XML ファイルのパス

    Returns:
        dict: {ソースファイルパス: {'lines': {行番号: ヒット数}, 'branches': {行番号: [(taken, total)]}}} の辞書
    """
    tree = ET.parse(xml_path)
    root = tree.getroot()

    # sources 要素からベースパスを取得
    sources = root.findall('.//source')
    base_path = sources[0].text if sources else ''

    coverage_data = {}

    for package in root.findall('.//package'):
        for cls in package.findall('.//class'):
            filename = cls.get('filename')
            # source + filename でフルパスを構成
            if base_path:
                # Windows のドライブレター (例: "D:") の場合
                if len(base_path) == 2 and base_path[1] == ':':
                    full_path = base_path + os.sep + filename
                else:
                    full_path = os.path.join(base_path, filename)
            else:
                full_path = filename

            # 行カバレッジと分岐カバレッジを抽出
            lines = {}
            branches = {}
            for line in cls.findall('.//line'):
                line_num = int(line.get('number'))
                hits = int(line.get('hits'))
                lines[line_num] = hits

                # 分岐カバレッジを抽出
                if line.get('branch') == 'true':
                    cov = parse_condition_coverage(line.get('condition-coverage'))
                    if cov[1] > 0:
                        branches[line_num] = cov

            coverage_data[full_path] = {'lines': lines, 'branches': branches}

    return coverage_data


def generate_lcov_info(coverage_data, output_file=None):
    """
    lcov info 形式のカバレッジ情報を生成する。

    lcov info 形式:
        TN:<テスト名>
        SF:<ソースファイルパス>
        DA:<行番号>,<実行回数>
        LF:<総行数>
        LH:<実行行数>
        BRDA:<行番号>,<ブロック番号>,<分岐番号>,<実行回数>
        BRF:<総分岐数>
        BRH:<実行分岐数>
        end_of_record

    Args:
        coverage_data: parse_cobertura の戻り値
        output_file: 出力ファイルパス (None の場合は標準出力)
    """
    lines_output = []

    for source_path, coverage_info in coverage_data.items():
        line_coverage = coverage_info['lines']
        branch_coverage = coverage_info.get('branches', {})

        # ファイルヘッダー
        lines_output.append("TN:")
        lines_output.append(f"SF:{source_path}")

        # 行カバレッジ
        total_lines = 0
        hit_lines = 0
        for line_num in sorted(line_coverage.keys()):
            hits = line_coverage[line_num]
            lines_output.append(f"DA:{line_num},{hits}")
            total_lines += 1
            if hits > 0:
                hit_lines += 1

        lines_output.append(f"LF:{total_lines}")
        lines_output.append(f"LH:{hit_lines}")

        # 分岐カバレッジ
        total_branches = 0
        hit_branches = 0
        for line_num in sorted(branch_coverage.keys()):
            covered, valid = branch_coverage[line_num]
            # lcov 形式では各分岐を個別に出力
            # BRDA:<行番号>,<ブロック番号>,<分岐番号>,<実行回数または->
            for branch_idx in range(valid):
                if branch_idx < covered:
                    # カバーされた分岐
                    lines_output.append(f"BRDA:{line_num},0,{branch_idx},1")
                    hit_branches += 1
                else:
                    # カバーされていない分岐
                    lines_output.append(f"BRDA:{line_num},0,{branch_idx},-")
                total_branches += 1

        if total_branches > 0:
            lines_output.append(f"BRF:{total_branches}")
            lines_output.append(f"BRH:{hit_branches}")

        lines_output.append("end_of_record")

    # 出力
    output_content = "\n".join(lines_output) + "\n"

    if output_file:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(output_content)
        print(f"Generated: {output_file}")
    else:
        print(output_content, end='')


def main():
    # 標準出力を UTF-8 に設定 (Windows 対応)
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        # Python 3.7 未満では reconfigure が存在しない
        pass

    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python cobertura2lcov.py <cobertura.xml> [output.info]",
              file=sys.stderr)
        sys.exit(1)

    xml_path = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) == 3 else None

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

    if not coverage_data:
        print("Warning: No coverage data found", file=sys.stderr)
        sys.exit(0)

    # lcov info を生成
    generate_lcov_info(coverage_data, output_file)


if __name__ == '__main__':
    main()
