#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cobertura_accumulate.py - Cobertura XML のカバレッジ情報を累積するスクリプト

使用方法:
    python cobertura_accumulate.py <current.xml> <accumulated.xml>

引数:
    current.xml      - 今回のテスト結果の Cobertura XML ファイル
    accumulated.xml  - 累積用の Cobertura XML ファイル

動作:
    - accumulated.xml が存在しない場合、current.xml の内容をコピーする
    - accumulated.xml が存在する場合、current.xml の hits を加算する
"""

import sys
import os
import shutil
import xml.etree.ElementTree as ET


def accumulate_coverage(current_path, accumulated_path):
    """
    2つの Cobertura XML のカバレッジ情報を累積する。

    Args:
        current_path: 今回のテスト結果の XML パス
        accumulated_path: 累積用の XML パス
    """
    # 累積ファイルが存在しない場合は、今回の結果をコピー
    if not os.path.exists(accumulated_path):
        shutil.copy(current_path, accumulated_path)
        print(f"Created: {accumulated_path}")
        return

    # 両方の XML をパース
    current_tree = ET.parse(current_path)
    accumulated_tree = ET.parse(accumulated_path)

    current_root = current_tree.getroot()
    accumulated_root = accumulated_tree.getroot()

    # 累積側の行情報を辞書化 (ファイル名 + 行番号でアクセス)
    accumulated_lines = {}
    for package in accumulated_root.findall('.//package'):
        for cls in package.findall('.//class'):
            filename = cls.get('filename')
            for line in cls.findall('.//line'):
                line_num = line.get('number')
                key = (filename, line_num)
                accumulated_lines[key] = line

    # 今回の結果を累積に加算
    for package in current_root.findall('.//package'):
        for cls in package.findall('.//class'):
            filename = cls.get('filename')
            for line in cls.findall('.//line'):
                line_num = line.get('number')
                current_hits = int(line.get('hits'))

                key = (filename, line_num)
                if key in accumulated_lines:
                    # 既存の行に hits を加算
                    acc_line = accumulated_lines[key]
                    acc_hits = int(acc_line.get('hits'))
                    acc_line.set('hits', str(acc_hits + current_hits))
                else:
                    # 新しい行の場合は警告 (通常は発生しない)
                    print(f"Warning: New line found in current: {filename}:{line_num}",
                          file=sys.stderr)

    # timestamp を今回の値で更新
    current_timestamp = current_root.get('timestamp')
    if current_timestamp:
        accumulated_root.set('timestamp', current_timestamp)

    # カバレッジ統計を再計算
    recalculate_coverage_stats(accumulated_root)

    # 結果を保存
    accumulated_tree.write(accumulated_path, encoding='utf-8', xml_declaration=True)
    print(f"Updated: {accumulated_path}")


def recalculate_coverage_stats(root):
    """
    hits の更新後、カバレッジ統計 (line-rate 等) を再計算する。

    Args:
        root: Cobertura XML のルート要素
    """
    total_lines = 0
    total_hits = 0

    for package in root.findall('.//package'):
        pkg_lines = 0
        pkg_hits = 0

        for cls in package.findall('.//class'):
            cls_lines = 0
            cls_hits = 0

            for line in cls.findall('.//line'):
                cls_lines += 1
                if int(line.get('hits')) > 0:
                    cls_hits += 1

            # class の line-rate を更新
            if cls_lines > 0:
                cls.set('line-rate', str(cls_hits / cls_lines))
            else:
                cls.set('line-rate', '0')

            pkg_lines += cls_lines
            pkg_hits += cls_hits

        # package の line-rate を更新
        if pkg_lines > 0:
            package.set('line-rate', str(pkg_hits / pkg_lines))
        else:
            package.set('line-rate', '0')

        total_lines += pkg_lines
        total_hits += pkg_hits

    # 全体の line-rate を更新
    if total_lines > 0:
        root.set('line-rate', str(total_hits / total_lines))
        root.set('lines-valid', str(total_lines))
        root.set('lines-covered', str(total_hits))
    else:
        root.set('line-rate', '0')


def main():
    if len(sys.argv) != 3:
        print("Usage: python cobertura_accumulate.py <current.xml> <accumulated.xml>",
              file=sys.stderr)
        sys.exit(1)

    current_path = sys.argv[1]
    accumulated_path = sys.argv[2]

    # 入力ファイルの存在確認
    if not os.path.exists(current_path):
        print(f"Error: Input file not found: {current_path}", file=sys.stderr)
        sys.exit(1)

    # 累積処理を実行
    try:
        accumulate_coverage(current_path, accumulated_path)
    except ET.ParseError as e:
        print(f"Error: Failed to parse XML: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
