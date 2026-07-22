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


# 構造コメント (// Arrange / // Pre-Assert / // Act / // Assert) のサイクル番号を判定する。
# 番号なしは 1、該当しない行 (// Cleanup を含む) は None を返す。
_PHASE_COMMENT_RE = re.compile(r'^\s*//\s*(Arrange|Pre-Assert|Act|Assert)(_([0-9]+))?\s*$')


def phase_cycle_number(line):
    """構造コメント行からサイクル番号を判定する。該当しない行は None を返す"""
    match = _PHASE_COMMENT_RE.match(line)
    if not match:
        return None
    if match.group(3) is not None:
        return int(match.group(3))
    return 1


def format_cycle_section(cycle, multi, state, act, pre_step, pre_chk, asrt_chk,
                          check_normal, check_abnormal, check_unspecified,
                          is_theory, param_count):
    """1 サイクル分のセクション (状態 / 手順 / 確認内容) を文字列で返す。multi が真のとき見出しに "_cycle" を付与する"""
    suffix = f"_{cycle}" if multi else ""
    out = []

    # --- 状態 ---
    out.append(f"\n### 状態{suffix}\n\n")
    for s in state:
        out.append(s + "\n")

    # --- 手順 (Act優先 → Pre-Assert) ---
    # 状態がある場合のみ前改行を入れる
    if len(state) > 0:
        out.append(f"\n### 手順{suffix}\n\n")
    else:
        out.append(f"### 手順{suffix}\n\n")
    for s in act:
        out.append(s + "\n")
    for s in pre_step:
        out.append(s + "\n")

    # --- 確認内容 (Pre-Assert → Assert) ---
    has_step = len(act) > 0 or len(pre_step) > 0
    # カテゴリ未指定のみかどうかを判定
    has_categorized = check_normal > 0 or check_abnormal > 0

    if not has_categorized:
        # カテゴリ未指定の件数のみを表示
        if is_theory and param_count > 1:
            check_header = f"### 確認内容{suffix} ({check_unspecified} * {param_count})\n\n"
        else:
            check_header = f"### 確認内容{suffix} ({check_unspecified})\n\n"
    else:
        # 正常系 / 異常系 / カテゴリ未指定をカテゴリ別に表示（Theory対応）
        categories = []

        if check_normal > 0:
            if is_theory and param_count > 1:
                categories.append(f"正常系:{check_normal} * {param_count}")
            else:
                categories.append(f"正常系:{check_normal}")

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

        check_header = f"### 確認内容{suffix} ({', '.join(categories)})\n\n"

    # 手順がある場合のみ前改行を入れる
    if has_step:
        out.append("\n" + check_header)
    else:
        out.append(check_header)
    for s in pre_chk:
        out.append(s + "\n")
    for s in asrt_chk:
        out.append(s + "\n")

    return "".join(out)


def insert_summary():
    """標準入力からテストコードを読み込み、サマリを挿入して標準出力"""
    lines = sys.stdin.readlines()

    # カテゴリ別の配列 (サイクル番号 -> リスト)
    state = {}
    act = {}
    pre_step = {}
    pre_chk = {}
    asrt_chk = {}

    # 確認内容のカテゴリ別カウンター (サイクル番号 -> カウント)
    check_normal = {}
    check_abnormal = {}
    check_unspecified = {}

    # サイクル管理 (// Arrange / // Pre-Assert / // Act / // Assert の番号増加で境界を検出)
    cur_cycle = 1
    max_cycle = 1

    # パラメータテスト検出用
    is_theory = False
    param_count = 0

    any_content = False

    # 各行を解析してタグを検出
    for line in lines:
        # [Theory] を検出
        if re.search(r'\[Theory\]', line):
            is_theory = True

        # [InlineData(...)] を検出してパラメータ数をカウント
        if re.search(r'\[InlineData\(', line):
            param_count += 1

        # サイクル境界の判定 (番号がそれまでの最大値を超えたときだけ更新する)
        ph_num = phase_cycle_number(line)
        if ph_num is not None and ph_num > cur_cycle:
            cur_cycle = ph_num
        if cur_cycle > max_cycle:
            max_cycle = cur_cycle

        # [状態]
        match = re.search(r'\[状態\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                state.setdefault(cur_cycle, []).append(s)
                any_content = True
            continue

        # [手順]
        match = re.search(r'\[手順\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                act.setdefault(cur_cycle, []).append(s)
                any_content = True
            continue

        # [Pre-Assert手順]
        match = re.search(r'\[Pre-Assert手順\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_step.setdefault(cur_cycle, []).append(s)
                any_content = True
            continue

        # [Pre-Assert確認_正常系]
        match = re.search(r'\[Pre-Assert確認_正常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.setdefault(cur_cycle, []).append(s)
                any_content = True
                if is_list_item(s):
                    check_normal[cur_cycle] = check_normal.get(cur_cycle, 0) + 1
            continue

        # [確認_正常系]
        match = re.search(r'\[確認_正常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.setdefault(cur_cycle, []).append(s)
                any_content = True
                if is_list_item(s):
                    check_normal[cur_cycle] = check_normal.get(cur_cycle, 0) + 1
            continue

        # [Pre-Assert確認_異常系]
        match = re.search(r'\[Pre-Assert確認_異常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.setdefault(cur_cycle, []).append(s)
                any_content = True
                if is_list_item(s):
                    check_abnormal[cur_cycle] = check_abnormal.get(cur_cycle, 0) + 1
            continue

        # [確認_異常系]
        match = re.search(r'\[確認_異常系\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.setdefault(cur_cycle, []).append(s)
                any_content = True
                if is_list_item(s):
                    check_abnormal[cur_cycle] = check_abnormal.get(cur_cycle, 0) + 1
            continue

        # [Pre-Assert確認] (カテゴリ未指定)
        match = re.search(r'\[Pre-Assert確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.setdefault(cur_cycle, []).append(s)
                any_content = True
                if is_list_item(s):
                    check_unspecified[cur_cycle] = check_unspecified.get(cur_cycle, 0) + 1
            continue

        # [確認] (カテゴリ未指定)
        match = re.search(r'\[確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.setdefault(cur_cycle, []).append(s)
                any_content = True
                if is_list_item(s):
                    check_unspecified[cur_cycle] = check_unspecified.get(cur_cycle, 0) + 1
            continue

    # サマリ項目が存在するかチェック
    has_summary = any_content

    # サマリを出力 (AWK版と完全に同じロジック)
    if has_summary:
        sys.stdout.write("## テスト項目\n")

        cycles = range(1, max_cycle + 1) if max_cycle >= 2 else [1]
        multi = max_cycle >= 2
        for cycle in cycles:
            sys.stdout.write(format_cycle_section(
                cycle, multi,
                state.get(cycle, []), act.get(cycle, []), pre_step.get(cycle, []),
                pre_chk.get(cycle, []), asrt_chk.get(cycle, []),
                check_normal.get(cycle, 0), check_abnormal.get(cycle, 0), check_unspecified.get(cycle, 0),
                is_theory, param_count))

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
