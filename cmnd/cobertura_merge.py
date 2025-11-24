#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cobertura_merge.py - 指定フォルダ以下の coverage.xml を合成するスクリプト

使用方法:
    python cobertura_merge.py <search_dir> [output.xml]

引数:
    search_dir  - coverage.xml を検索するルートディレクトリ
    output.xml  - 合成結果の出力先ファイル (省略時: search_dir/coverage.xml)

動作:
    - search_dir 以下を再帰的に検索し、すべての coverage.xml を収集する
    - 出力先ファイル自体は合成対象から除外する
    - 各ファイルのカバレッジ情報を合成 (同一ファイル・同一行の hits を加算)
    - 合成結果を output.xml に出力する
"""

import sys
import os
import re
import xml.etree.ElementTree as ET
from pathlib import Path


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


def find_coverage_files(search_dir):
    """
    指定ディレクトリ以下の coverage.xml を再帰的に検索する。

    Args:
        search_dir: 検索ルートディレクトリ

    Returns:
        list: coverage.xml ファイルパスのリスト
    """
    coverage_files = []
    for root, dirs, files in os.walk(search_dir):
        for filename in files:
            if filename == 'coverage.xml':
                coverage_files.append(os.path.join(root, filename))
    return coverage_files


def get_common_source_prefix(sources):
    """
    複数の source パスから共通のプレフィックスを取得する。

    Args:
        sources: source パスのリスト

    Returns:
        共通のディレクトリパス
    """
    if not sources:
        return ""
    if len(sources) == 1:
        return sources[0]

    # パスを分割して共通部分を見つける
    split_paths = [s.rstrip('/').split('/') for s in sources]
    common_parts = []

    for parts in zip(*split_paths):
        if len(set(parts)) == 1:
            common_parts.append(parts[0])
        else:
            break

    return '/'.join(common_parts)


def normalize_filename(source, filename, common_source):
    """
    source と filename を組み合わせて、共通 source からの相対パスに正規化する。

    Args:
        source: 元の source パス
        filename: 元の filename
        common_source: 共通の source パス

    Returns:
        正規化された filename
    """
    # source + filename で完全パスを構築
    if filename.startswith('/'):
        full_path = filename
    else:
        full_path = os.path.join(source, filename)

    # 共通 source からの相対パスを取得
    if common_source and full_path.startswith(common_source):
        relative = full_path[len(common_source):].lstrip('/')
        return relative if relative else filename

    return filename


def merge_coverage_files(coverage_files):
    """
    複数の Cobertura XML ファイルを合成する。

    Args:
        coverage_files: coverage.xml ファイルパスのリスト

    Returns:
        ElementTree: 合成された XML ツリー
    """
    if not coverage_files:
        return None

    # 全ファイルの source を収集
    all_sources = []
    for xml_path in coverage_files:
        try:
            tree = ET.parse(xml_path)
            root = tree.getroot()
            sources = root.findall('.//sources/source')
            for source in sources:
                if source.text:
                    all_sources.append(source.text)
        except ET.ParseError:
            continue

    # 共通 source を計算
    common_source = get_common_source_prefix(all_sources)

    # 最初のファイルをベースとして使用
    base_tree = ET.parse(coverage_files[0])
    base_root = base_tree.getroot()

    # ベースの source を取得
    base_source_elem = base_root.find('.//sources/source')
    base_source = base_source_elem.text if base_source_elem is not None and base_source_elem.text else ""

    # ベースの source を共通 source に更新
    if base_source_elem is not None:
        base_source_elem.text = common_source

    # ベースの行情報を辞書化
    # キー: (package_name, normalized_filename, line_number)
    # 値: line 要素への参照
    merged_lines = {}
    merged_classes = {}  # (package_name, normalized_filename) -> class 要素
    merged_packages = {}  # package_name -> package 要素

    # 最大 timestamp を追跡
    max_timestamp = int(base_root.get('timestamp', '0'))

    # ベースのパッケージを取得 (新しいパッケージ追加用)
    base_packages = base_root.find('.//packages')
    if base_packages is None:
        base_packages = ET.SubElement(base_root, 'packages')

    for package in base_root.findall('.//package'):
        pkg_name = package.get('name')
        merged_packages[pkg_name] = package
        for cls in package.findall('.//class'):
            orig_filename = cls.get('filename')
            norm_filename = normalize_filename(base_source, orig_filename, common_source)
            # filename を正規化されたものに更新
            cls.set('filename', norm_filename)
            merged_classes[(pkg_name, norm_filename)] = cls
            for line in cls.findall('.//line'):
                line_num = line.get('number')
                key = (pkg_name, norm_filename, line_num)
                merged_lines[key] = line

    # 2番目以降のファイルを合成
    for xml_path in coverage_files[1:]:
        try:
            tree = ET.parse(xml_path)
            root = tree.getroot()
        except ET.ParseError as e:
            print(f"Warning: Failed to parse {xml_path}: {e}", file=sys.stderr)
            continue

        # このファイルの source を取得
        file_source_elem = root.find('.//sources/source')
        file_source = file_source_elem.text if file_source_elem is not None and file_source_elem.text else ""

        # timestamp の最大値を更新
        timestamp = int(root.get('timestamp', '0'))
        if timestamp > max_timestamp:
            max_timestamp = timestamp

        for package in root.findall('.//package'):
            pkg_name = package.get('name')

            # 新しいパッケージの場合、追加
            if pkg_name not in merged_packages:
                new_pkg = ET.SubElement(base_packages, 'package')
                new_pkg.set('name', pkg_name)
                new_pkg.set('line-rate', '0')
                new_pkg.set('branch-rate', '0')
                new_pkg.set('complexity', '0')
                ET.SubElement(new_pkg, 'classes')
                merged_packages[pkg_name] = new_pkg

            target_package = merged_packages[pkg_name]

            for cls in package.findall('.//class'):
                orig_filename = cls.get('filename')
                filename = normalize_filename(file_source, orig_filename, common_source)
                cls_key = (pkg_name, filename)

                if cls_key not in merged_classes:
                    # 新しいファイルの場合、クラスを追加
                    classes_elem = target_package.find('classes')
                    if classes_elem is None:
                        classes_elem = ET.SubElement(target_package, 'classes')

                    new_cls = ET.SubElement(classes_elem, 'class')
                    new_cls.set('name', cls.get('name'))
                    new_cls.set('filename', filename)
                    new_cls.set('line-rate', '0')
                    new_cls.set('branch-rate', '0')
                    new_cls.set('complexity', '0')

                    ET.SubElement(new_cls, 'methods')
                    ET.SubElement(new_cls, 'lines')

                    merged_classes[cls_key] = new_cls

                for line in cls.findall('.//line'):
                    line_num = line.get('number')
                    hits = int(line.get('hits'))
                    key = (pkg_name, filename, line_num)

                    if key in merged_lines:
                        # 既存の行に hits を加算
                        existing_line = merged_lines[key]
                        existing_hits = int(existing_line.get('hits'))
                        existing_line.set('hits', str(existing_hits + hits))

                        # condition-coverage の累積 (branch="true" の行のみ)
                        if line.get('branch') == 'true' and existing_line.get('branch') == 'true':
                            curr_cov = parse_condition_coverage(line.get('condition-coverage'))
                            acc_cov = parse_condition_coverage(existing_line.get('condition-coverage'))

                            if curr_cov[1] > 0 and acc_cov[1] > 0:
                                # 同じ valid 数を想定し、covered は最大値を取る
                                new_covered = max(curr_cov[0], acc_cov[0])
                                valid = acc_cov[1]
                                if valid > 0:
                                    pct = int(100 * new_covered / valid)
                                    existing_line.set('condition-coverage',
                                                   f"{pct}% ({new_covered}/{valid})")

                                # conditions 要素内の coverage も更新
                                for acc_cond in existing_line.findall('.//condition'):
                                    cond_num = acc_cond.get('number')
                                    for curr_cond in line.findall('.//condition'):
                                        if curr_cond.get('number') == cond_num:
                                            acc_pct_str = acc_cond.get('coverage', '0%').rstrip('%')
                                            curr_pct_str = curr_cond.get('coverage', '0%').rstrip('%')
                                            try:
                                                acc_pct = int(acc_pct_str)
                                                curr_pct = int(curr_pct_str)
                                                acc_cond.set('coverage', f"{max(acc_pct, curr_pct)}%")
                                            except ValueError:
                                                pass
                                            break
                    else:
                        # 新しい行の場合
                        target_cls = merged_classes[cls_key]
                        lines_elem = target_cls.find('lines')
                        if lines_elem is None:
                            lines_elem = ET.SubElement(target_cls, 'lines')
                        new_line = ET.SubElement(lines_elem, 'line')
                        new_line.set('number', line_num)
                        new_line.set('hits', str(hits))
                        # branch 属性をコピー
                        if line.get('branch'):
                            new_line.set('branch', line.get('branch'))
                        if line.get('condition-coverage'):
                            new_line.set('condition-coverage', line.get('condition-coverage'))
                        # conditions 要素をコピー
                        for cond in line.findall('.//condition'):
                            conditions_elem = new_line.find('conditions')
                            if conditions_elem is None:
                                conditions_elem = ET.SubElement(new_line, 'conditions')
                            new_cond = ET.SubElement(conditions_elem, 'condition')
                            for attr in cond.attrib:
                                new_cond.set(attr, cond.get(attr))
                        merged_lines[key] = new_line

    # timestamp を最大値に設定
    base_root.set('timestamp', str(max_timestamp))

    # カバレッジ統計を再計算
    recalculate_coverage_stats(base_root)

    return base_tree


def indent_xml(elem, level=0):
    """
    XML 要素を整形 (インデント) する。

    Args:
        elem: XML 要素
        level: 現在のインデントレベル
    """
    indent = "\n" + "  " * level
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = indent + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = indent
        for child in elem:
            indent_xml(child, level + 1)
        if not child.tail or not child.tail.strip():
            child.tail = indent
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = indent


def recalculate_coverage_stats(root):
    """
    hits の更新後、カバレッジ統計 (line-rate, branch-rate 等) を再計算する。

    Args:
        root: Cobertura XML のルート要素
    """
    total_lines = 0
    total_hits = 0
    total_branches_valid = 0
    total_branches_covered = 0

    for package in root.findall('.//package'):
        pkg_lines = 0
        pkg_hits = 0
        pkg_branches_valid = 0
        pkg_branches_covered = 0

        for cls in package.findall('.//class'):
            cls_lines = 0
            cls_hits = 0
            cls_branches_valid = 0
            cls_branches_covered = 0

            for line in cls.findall('.//line'):
                cls_lines += 1
                if int(line.get('hits')) > 0:
                    cls_hits += 1

                # branch カバレッジの集計
                if line.get('branch') == 'true':
                    cov = parse_condition_coverage(line.get('condition-coverage'))
                    cls_branches_covered += cov[0]
                    cls_branches_valid += cov[1]

            # class の line-rate を更新
            if cls_lines > 0:
                cls.set('line-rate', str(cls_hits / cls_lines))
            else:
                cls.set('line-rate', '0')

            # class の branch-rate を更新 (存在する場合のみ)
            if cls_branches_valid > 0:
                cls.set('branch-rate', str(cls_branches_covered / cls_branches_valid))
            elif cls.get('branch-rate') is not None:
                cls.set('branch-rate', '1.0')

            pkg_lines += cls_lines
            pkg_hits += cls_hits
            pkg_branches_valid += cls_branches_valid
            pkg_branches_covered += cls_branches_covered

        # package の line-rate を更新
        if pkg_lines > 0:
            package.set('line-rate', str(pkg_hits / pkg_lines))
        else:
            package.set('line-rate', '0')

        # package の branch-rate を更新 (存在する場合のみ)
        if pkg_branches_valid > 0:
            package.set('branch-rate', str(pkg_branches_covered / pkg_branches_valid))
        elif package.get('branch-rate') is not None:
            package.set('branch-rate', '1.0')

        total_lines += pkg_lines
        total_hits += pkg_hits
        total_branches_valid += pkg_branches_valid
        total_branches_covered += pkg_branches_covered

    # 全体の line-rate を更新
    if total_lines > 0:
        root.set('line-rate', str(total_hits / total_lines))
        root.set('lines-valid', str(total_lines))
        root.set('lines-covered', str(total_hits))
    else:
        root.set('line-rate', '0')

    # 全体の branch-rate を更新 (存在する場合のみ)
    if total_branches_valid > 0:
        root.set('branch-rate', str(total_branches_covered / total_branches_valid))
        root.set('branches-valid', str(total_branches_valid))
        root.set('branches-covered', str(total_branches_covered))
    elif root.get('branch-rate') is not None:
        root.set('branch-rate', '1.0')


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python cobertura_merge.py <search_dir> [output.xml]",
              file=sys.stderr)
        sys.exit(1)

    search_dir = sys.argv[1]
    if len(sys.argv) == 3:
        output_path = sys.argv[2]
    else:
        output_path = os.path.join(search_dir, 'coverage.xml')

    # 出力パスを絶対パスに変換 (除外判定用)
    output_path_abs = os.path.abspath(output_path)

    # 検索ディレクトリの存在確認
    if not os.path.isdir(search_dir):
        print(f"Error: Directory not found: {search_dir}", file=sys.stderr)
        sys.exit(1)

    # coverage.xml を検索
    coverage_files = find_coverage_files(search_dir)

    # 出力先ファイルを合成対象から除外
    coverage_files = [f for f in coverage_files if os.path.abspath(f) != output_path_abs]

    if not coverage_files:
        print(f"Error: No coverage.xml found in {search_dir}", file=sys.stderr)
        sys.exit(1)

    print(f"Found {len(coverage_files)} coverage.xml file(s):")
    for f in coverage_files:
        print(f"  - {f}")

    # 合成処理を実行
    try:
        merged_tree = merge_coverage_files(coverage_files)
    except ET.ParseError as e:
        print(f"Error: Failed to parse XML: {e}", file=sys.stderr)
        sys.exit(1)

    if merged_tree is None:
        print("Error: Failed to merge coverage files", file=sys.stderr)
        sys.exit(1)

    # 出力ディレクトリの作成
    output_dir = os.path.dirname(output_path)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    # XML を整形
    indent_xml(merged_tree.getroot())

    # 結果を保存
    merged_tree.write(output_path, encoding='utf-8', xml_declaration=True)
    print(f"Merged coverage written to: {output_path}")


if __name__ == '__main__':
    main()
