#!/usr/bin/env python3
"""
バッチ dotnet test 出力から個別テスト結果を抽出するスクリプト

バッチ実行された dotnet test のコンソール出力から、指定テストに該当する行を抽出し、
従来の個別実行時と同等のフォーマットで出力する。

使用方法:
    extract_dotnet_output.py <batch_output_file> <test_id> <result>

Args:
    batch_output_file: バッチ dotnet test の出力ファイルパス
    test_id: テスト識別子 (例: CalcLibraryTests.Add_ShouldReturnCorrectResult)
    result: テスト結果 (Passed または Failed)

Output:
    従来の個別実行時と同等のフォーマットでテスト結果を標準出力
"""

import sys
import re


def extract_output(batch_file, test_id, result):
    with open(batch_file, 'r', encoding='utf-8', errors='replace') as f:
        lines = f.readlines()

    # test_id からクラス名とメソッド名を分離
    parts = test_id.split(".", 1)
    class_name = parts[0] if len(parts) > 0 else ""
    method_name = parts[1] if len(parts) > 1 else ""

    # ヘッダー行を抽出 (dll パス、VSTest バージョン、テスト開始メッセージ、ファイル数)
    header_lines = []
    for line in lines:
        # dll パス行
        if re.search(r'\.dll\s+\(', line):
            header_lines.append(line)
        # VSTest バージョン行
        elif re.search(r'VSTest', line) and re.search(r'バージョン|version', line, re.IGNORECASE):
            header_lines.append(line)
            header_lines.append("\n")
        # テスト開始メッセージ
        elif re.search(r'テスト実行を開始しています|Starting test execution', line):
            header_lines.append(line)
        # ファイル数行
        elif re.search(r'個のテスト ファイルが指定されたパターンと一致しました|test file.*matched', line, re.IGNORECASE):
            header_lines.append(line)

    # コマンド行を出力
    sys.stdout.write(f'dotnet test --filter "FullyQualifiedName~{test_id}"\n')
    sys.stdout.write("\n")

    # ヘッダー行を出力
    for line in header_lines:
        sys.stdout.write(line)

    # 該当テストの結果行を抽出
    # パターン: "  成功|失敗|Passed|Failed テスト名" にマッチ
    test_result_lines = []
    in_failure_block = False
    fqn_pattern = re.compile(
        r'(成功|失敗|Passed|Failed)\s+\S*' + re.escape(class_name) + r'\.' + re.escape(method_name)
    )

    for i, line in enumerate(lines):
        if fqn_pattern.search(line):
            test_result_lines.append(line)
            in_failure_block = False
            # 失敗の場合、後続のスタックトレース等を収集
            if re.search(r'失敗|Failed', line):
                in_failure_block = True
        elif in_failure_block:
            # スタックトレースや詳細行 (インデント付き)
            if re.match(r'^\s{4,}', line) or re.match(r'^\s*$', line):
                test_result_lines.append(line)
            else:
                in_failure_block = False

    # テスト結果行を出力
    for line in test_result_lines:
        sys.stdout.write(line)

    # テスト結果数をカウント
    passed_count = 0
    failed_count = 0
    for line in test_result_lines:
        if re.search(r'^\s*(成功|Passed)\s', line):
            passed_count += 1
        elif re.search(r'^\s*(失敗|Failed)\s', line):
            failed_count += 1
    total_count = passed_count + failed_count

    # フッターを出力
    sys.stdout.write("\n")
    if result == "Passed":
        sys.stdout.write("テストの実行に成功しました。\n")
    else:
        sys.stdout.write("テストの実行に失敗しました。\n")

    sys.stdout.write(f"テストの合計数: {total_count}\n")
    if passed_count > 0:
        sys.stdout.write(f"     成功: {passed_count}\n")
    if failed_count > 0:
        sys.stdout.write(f"     失敗: {failed_count}\n")

    # 合計時間をバッチ出力から取得
    for line in lines:
        if re.search(r'合計時間|Total time', line, re.IGNORECASE):
            sys.stdout.write(line)
            break


def main():
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        pass

    if len(sys.argv) != 4:
        print("Usage: extract_dotnet_output.py <batch_output_file> <test_id> <result>",
              file=sys.stderr)
        sys.exit(1)

    batch_file = sys.argv[1]
    test_id = sys.argv[2]
    result = sys.argv[3]

    extract_output(batch_file, test_id, result)


if __name__ == '__main__':
    main()
