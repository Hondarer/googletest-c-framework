#!/usr/bin/env python3
"""
GoogleTest の出力テキストに ANSI カラーコードを付与します。

エスケープシーケンスなしの GoogleTest 出力を受け取り、
見出し部 (角括弧で囲まれた部分) に適切な色を付けて出力します。

使用例:
    test.exe | python add_gtest_color.py
    cat test_output.txt | python add_gtest_color.py
    python add_gtest_color.py < test_output.txt
"""

import re
import sys


class AnsiColor:
    """ANSI カラーコード定義"""

    RESET = "\033[0m"
    GREEN = "\033[0;32m"
    RED = "\033[0;31m"
    YELLOW = "\033[0;33m"


# 見出しパターンと色の対応
# タプル形式: (正規表現パターン, 色)
#
# GoogleTest ソースコード (gtest.cc) の PrettyUnitTestResultPrinter に基づく。
# 着色は見出し部 (角括弧内) のみに適用され、テスト名や本文には適用されない。
#
# 注意: 元の gtest は result()->Passed(), result()->Failed(), result()->Skipped() の
# 戻り値に応じて色を決定する。テキストベースのフィルターでは見出し文字列で判定するため、
# 完全な可逆性は保証されないが、標準出力においては実用上問題ない。
COLOR_RULES = [
    # 赤: 失敗 (OnTestEnd で result()->Failed() の場合、PrintFailedTests)
    (re.compile(r"^\[\s*FAILED\s*\]"), AnsiColor.RED),

    # 黄: スキップ (OnTestEnd で result()->Skipped() の場合、PrintSkippedTests)
    (re.compile(r"^\[\s*SKIPPED\s*\]"), AnsiColor.YELLOW),

    # 黄: Death test 実行中 (DeathTest 内部で使用)
    (re.compile(r"^\[\s*DEATH\s*\]"), AnsiColor.YELLOW),

    # 黄: 警告 (GTEST_FLAG の警告出力など)
    (re.compile(r"^\[\s*WARNING\s*\]"), AnsiColor.YELLOW),

    # 緑: 成功 (OnTestEnd で result()->Passed() の場合)
    (re.compile(r"^\[\s*OK\s*\]"), AnsiColor.GREEN),

    # 緑: 成功サマリ (OnTestProgramEnd)
    (re.compile(r"^\[\s*PASSED\s*\]"), AnsiColor.GREEN),

    # 緑: テスト開始 (OnTestStart)
    (re.compile(r"^\[\s*RUN\s*\]"), AnsiColor.GREEN),

    # 緑: プログラム開始/終了 (OnTestProgramStart, OnTestProgramEnd)
    (re.compile(r"^\[=+\]"), AnsiColor.GREEN),

    # 緑: テストスイート開始/終了 (OnTestSuiteStart, OnTestSuiteEnd)
    (re.compile(r"^\[-+\]"), AnsiColor.GREEN),

    # 緑: グローバル環境 (OnEnvironmentsSetUpStart 等)
    # ※ [----------] と同じパターンでカバー済み
]

# 見出し部分を抽出する正規表現
HEADER_PATTERN = re.compile(r"^(\[[^\]]+\])(.*)$")


def colorize_line(line: str) -> str:
    """
    1 行のテキストに色を付けます。

    Args:
        line: 入力行

    Returns:
        色付けされた行 (該当しない場合はそのまま)
    """
    for pattern, color in COLOR_RULES:
        if pattern.match(line):
            match = HEADER_PATTERN.match(line)
            if match:
                header = match.group(1)
                rest = match.group(2)
                return f"{color}{header}{AnsiColor.RESET}{rest}"
    return line


def enable_windows_ansi() -> None:
    """Windows コンソールで ANSI エスケープシーケンスを有効にします"""
    if sys.platform != "win32":
        return

    try:
        import ctypes

        kernel32 = ctypes.windll.kernel32
        # STD_OUTPUT_HANDLE = -11
        handle = kernel32.GetStdHandle(-11)
        # ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004
        mode = ctypes.c_ulong()
        kernel32.GetConsoleMode(handle, ctypes.byref(mode))
        kernel32.SetConsoleMode(handle, mode.value | 0x0004)
    except Exception:
        pass  # 失敗しても処理を続行


def main() -> None:
    # 標準入出力を UTF-8 に設定 (Windows 対応)
    try:
        sys.stdin.reconfigure(encoding='utf-8')
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        # Python 3.7 未満では reconfigure が存在しない
        pass

    enable_windows_ansi()

    for line in sys.stdin:
        # 末尾の改行を保持しつつ処理
        line_stripped = line.rstrip("\n\r")
        colored = colorize_line(line_stripped)
        print(colored)


if __name__ == "__main__":
    main()
