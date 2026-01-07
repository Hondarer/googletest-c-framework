#!/usr/bin/env python3
"""
テストコードからサマリを生成して挿入するスクリプト

insert_summary_c_cpp.awk の Python 移植版 (.NET テスト用)

使用方法:
    cat test_code.cs | insert_summary_dotnet.py

Input:
    テストコード (標準入力)

Output:
    サマリ付きテストコード (標準出力)
    - ## テスト項目
    - ### 状態
    - ### 手順
    - ### 確認内容 (N)
    - ----
    - 元のテストコード

タグ:
    [状態] - テストの前提条件
    [手順] - 実行手順 (Act)
    [Pre-Assert手順] - Assert 前の手順
    [確認] - Assert による確認内容
    [Pre-Assert確認] - Pre-Assert による確認

Example:
    python3 get_test_code_dotnet.py CalcLibraryTests.cs CalcLibraryTests Add_ShouldReturnCorrectResult | \\
        python3 insert_summary_dotnet.py
"""

import sys
import re


def trim(s):
    """先頭の空白1つと末尾の空白群・改行を削除"""
    s = re.sub(r'^ ', '', s, count=1)
    s = re.sub(r'[ \t\r\n]+$', '', s)
    return s


def is_list_item(s):
    """リスト項目かどうかを判定"""
    return bool(re.match(r'^[-*+]|^[0-9]+\.', s))


def insert_summary():
    """標準入力からテストコードを読み込み、サマリを挿入して標準出力"""
    lines = sys.stdin.readlines()

    # カテゴリ別の配列
    state = []
    act = []
    pre_step = []
    pre_chk = []
    asrt_chk = []

    # 確認内容のカテゴリ別カウンター
    check_normal = 0
    check_semi_normal = 0
    check_abnormal = 0
    check_unspecified = 0

    # パラメータテスト検出用
    is_theory = False
    param_count = 0

    # 各行を解析してタグを検出
    for line in lines:
        # [Theory] を検出
        if re.search(r'\[Theory\]', line):
            is_theory = True

        # [InlineData(...)] を検出してパラメータ数をカウント
        if re.search(r'\[InlineData\(', line):
            param_count += 1

        # [状態]
        match = re.search(r'\[状態\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                state.append(s)
            continue

        # [手順]
        match = re.search(r'\[手順\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                act.append(s)
            continue

        # [Pre-Assert手順]
        match = re.search(r'\[Pre-Assert手順\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_step.append(s)
            continue

        # [Pre-Assert確認_正常系]
        match = re.search(r'\[Pre-Assert確認_正常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.append(s)
                if is_list_item(s):
                    check_normal += 1
            continue

        # [確認_正常系]
        match = re.search(r'\[確認_正常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.append(s)
                if is_list_item(s):
                    check_normal += 1
            continue

        # [Pre-Assert確認_準正常系]
        match = re.search(r'\[Pre-Assert確認_準正常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.append(s)
                if is_list_item(s):
                    check_semi_normal += 1
            continue

        # [確認_準正常系]
        match = re.search(r'\[確認_準正常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.append(s)
                if is_list_item(s):
                    check_semi_normal += 1
            continue

        # [Pre-Assert確認_異常系]
        match = re.search(r'\[Pre-Assert確認_異常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.append(s)
                if is_list_item(s):
                    check_abnormal += 1
            continue

        # [確認_異常系]
        match = re.search(r'\[確認_異常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.append(s)
                if is_list_item(s):
                    check_abnormal += 1
            continue

        # [Pre-Assert確認] (カテゴリ未指定)
        match = re.search(r'\[Pre-Assert確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.append(s)
                if is_list_item(s):
                    check_unspecified += 1
            continue

        # [確認] (カテゴリ未指定)
        match = re.search(r'\[確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.append(s)
                if is_list_item(s):
                    check_unspecified += 1
            continue

    # サマリ項目が存在するかチェック
    has_summary = len(state) > 0 or len(act) > 0 or len(pre_step) > 0 or len(pre_chk) > 0 or len(asrt_chk) > 0

    # サマリを出力 (AWK版と完全に同じロジック)
    if has_summary:
        sys.stdout.write("## テスト項目\n")

        # --- 状態 ---
        sys.stdout.write("\n### 状態\n\n")
        for s in state:
            sys.stdout.write(s + "\n")

        # --- 手順 (Act優先 → Pre-Assert) ---
        # 状態がある場合のみ前改行を入れる
        if len(state) > 0:
            sys.stdout.write("\n### 手順\n\n")
        else:
            sys.stdout.write("### 手順\n\n")
        for s in act:
            sys.stdout.write(s + "\n")
        for s in pre_step:
            sys.stdout.write(s + "\n")

        # --- 確認内容 (Pre-Assert → Assert) ---
        # カテゴリ未指定のみかどうかを判定
        has_categorized = check_normal > 0 or check_semi_normal > 0 or check_abnormal > 0

        if not has_categorized:
            # 後方互換モード: カテゴリ未指定のみの場合
            if is_theory and param_count > 1:
                check_header = f"### 確認内容 ({check_unspecified} * {param_count})\n\n"
            else:
                check_header = f"### 確認内容 ({check_unspecified})\n\n"
        else:
            # 新フォーマット: カテゴリ別表示（Theory対応）
            categories = []

            if check_normal > 0:
                if is_theory and param_count > 1:
                    categories.append(f"正常系:{check_normal} * {param_count}")
                else:
                    categories.append(f"正常系:{check_normal}")

            if check_semi_normal > 0:
                if is_theory and param_count > 1:
                    categories.append(f"準正常系:{check_semi_normal} * {param_count}")
                else:
                    categories.append(f"準正常系:{check_semi_normal}")

            if check_abnormal > 0:
                if is_theory and param_count > 1:
                    categories.append(f"異常系:{check_abnormal} * {param_count}")
                else:
                    categories.append(f"異常系:{check_abnormal}")

            if check_unspecified > 0:
                if is_theory and param_count > 1:
                    categories.append(f"カテゴリ未指定:{check_unspecified} * {param_count}")
                else:
                    categories.append(f"カテゴリ未指定:{check_unspecified}")

            check_header = f"### 確認内容 ({', '.join(categories)})\n\n"

        # 手順がある場合のみ前改行を入れる
        if len(act) > 0 or len(pre_step) > 0:
            sys.stdout.write("\n" + check_header)
        else:
            sys.stdout.write(check_header)
        for s in pre_chk:
            sys.stdout.write(s + "\n")
        for s in asrt_chk:
            sys.stdout.write(s + "\n")

        sys.stdout.write("----\n")

    # 元のコードを出力 (共通の字下げを削除)
    # 最小の先頭空白数を検出
    min_indent = None
    for line in lines:
        if line.strip():  # 空行でない場合
            # 先頭の空白数を数える
            indent = len(line) - len(line.lstrip(' '))
            if min_indent is None or indent < min_indent:
                min_indent = indent

    # 共通の字下げを削除して出力
    if min_indent is None:
        min_indent = 0

    for line in lines:
        if line.strip():  # 空行でない場合
            # min_indent 分の空白を削除
            sys.stdout.write(line[min_indent:])
        else:
            # 空行はそのまま出力
            sys.stdout.write(line)


def main():
    # 標準入出力を UTF-8 に設定 (Windows 対応)
    try:
        sys.stdin.reconfigure(encoding='utf-8')
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        # Python 3.7 未満では reconfigure が存在しない
        pass

    insert_summary()


if __name__ == '__main__':
    main()
