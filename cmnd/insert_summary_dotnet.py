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
    check_count = 0

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

        # [Pre-Assert確認]
        match = re.search(r'\[Pre-Assert確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.append(s)
                if is_list_item(s):
                    check_count += 1
            continue

        # [確認]
        match = re.search(r'\[確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.append(s)
                if is_list_item(s):
                    check_count += 1
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
        # パラメータテストの場合は (確認数 * パラメータ数) 形式で表示
        if is_theory and param_count > 1:
            check_header = f"### 確認内容 ({check_count} * {param_count})\n\n"
        else:
            check_header = f"### 確認内容 ({check_count})\n\n"

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
    # Windows で標準入出力を UTF-8 に設定
    sys.stdin.reconfigure(encoding='utf-8')
    sys.stdout.reconfigure(encoding='utf-8')
    sys.stderr.reconfigure(encoding='utf-8')

    insert_summary()


if __name__ == '__main__':
    main()
