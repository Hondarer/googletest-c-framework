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
COLOR_RULES = [
    # 赤: 失敗
    (re.compile(r"^\[\s*FAILED\s*\]"), AnsiColor.RED),
    # 黄: スキップ、Death test、情報
    (re.compile(r"^\[\s*SKIPPED\s*\]"), AnsiColor.YELLOW),
    (re.compile(r"^\[\s*DEATH\s*\]"), AnsiColor.YELLOW),
    (re.compile(r"^\[\s*INFO\s*\]"), AnsiColor.YELLOW),
    # 緑: 成功、実行中、その他の標準見出し
    (re.compile(r"^\[\s*PASSED\s*\]"), AnsiColor.GREEN),
    (re.compile(r"^\[\s*OK\s*\]"), AnsiColor.GREEN),
    (re.compile(r"^\[\s*RUN\s*\]"), AnsiColor.GREEN),
    (re.compile(r"^\[=+\]"), AnsiColor.GREEN),
    (re.compile(r"^\[-+\]"), AnsiColor.GREEN),
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
    enable_windows_ansi()

    for line in sys.stdin:
        # 末尾の改行を保持しつつ処理
        line_stripped = line.rstrip("\n\r")
        colored = colorize_line(line_stripped)
        print(colored)


if __name__ == "__main__":
    main()
