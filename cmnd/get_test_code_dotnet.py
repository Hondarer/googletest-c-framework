#!/usr/bin/env python3
"""
.NET テストファイルからテストメソッドを抽出するスクリプト

使用方法:
    get_test_code_dotnet.py <file_path> <class_name> <method_name>

Args:
    file_path: テストファイルのパス (.cs)
    class_name: テストクラス名
    method_name: テストメソッド名

Output:
    抽出されたテストコード (標準出力)
    - メソッド直前のコメント (XML コメント、通常のコメント)
    - 属性 ([Fact], [Theory], [InlineData] など)
    - メソッド本体

Example:
    python3 get_test_code_dotnet.py CalcLibraryTests.cs CalcLibraryTests Add_ShouldReturnCorrectResult
"""

import sys
import re


def extract_test_code(file_path, class_name, method_name):
    """
    .NET テストファイルからテストメソッドを抽出する

    Args:
        file_path: テストファイルのパス
        class_name: テストクラス名
        method_name: テストメソッド名
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: Failed to read file '{file_path}': {e}", file=sys.stderr)
        sys.exit(1)

    # クラス内部のフラグ
    in_class = False
    # メソッド検出フラグ
    in_method = False
    # 括弧のカウント
    brace_count = 0
    # バッファ (コメント・属性用)
    buffer = []
    # 出力フラグ
    output_started = False
    # 複数行コメント中フラグ
    in_multiline_comment = False

    for line in lines:
        # クラス定義を検出
        if re.search(rf'class\s+{re.escape(class_name)}\s*', line):
            in_class = True
            continue

        if not in_class:
            continue

        # 複数行コメントの開始と終了が同じ行にある場合
        if re.search(r'/\*.*\*/', line):
            if not in_method:
                buffer.append(line)
            else:
                sys.stdout.write(line)
            continue

        # 複数行コメントの開始
        if re.search(r'/\*', line) and not in_multiline_comment:
            in_multiline_comment = True
            if not in_method:
                buffer.append(line)
            else:
                sys.stdout.write(line)
            continue

        # 複数行コメント中
        if in_multiline_comment:
            if not in_method:
                buffer.append(line)
            else:
                sys.stdout.write(line)
            # 複数行コメントの終了
            if re.search(r'\*/', line):
                in_multiline_comment = False
            continue

        # コメント行をバッファに追加 (XML コメント /// を含む)
        if re.match(r'^\s*(//|///)', line):
            if not in_method:
                buffer.append(line)
            else:
                sys.stdout.write(line)
            continue

        # #pragma をバッファに追加
        if re.match(r'^\s*#pragma', line):
            if not in_method:
                buffer.append(line)
            else:
                sys.stdout.write(line)
            continue

        # 空行でバッファをクリア (メソッド検出前のみ、複数行コメント中を除く)
        if re.match(r'^\s*$', line):
            if not in_method and not in_multiline_comment:
                # 属性が既にバッファにある場合はクリアしない
                # (属性とメソッド定義の間に空行がある場合に対応)
                has_attribute = any(re.match(r'^\s*\[', buf_line) for buf_line in buffer)
                if not has_attribute:
                    buffer = []
            if in_method:
                sys.stdout.write(line)
            continue

        # 属性行をバッファに追加
        if re.match(r'^\s*\[', line):
            if not in_method:
                buffer.append(line)
            else:
                sys.stdout.write(line)
            continue

        # メソッド定義を検出
        if re.search(rf'\s+{re.escape(method_name)}\s*\(', line):
            in_method = True
            output_started = True
            # バッファの内容を出力
            for buf_line in buffer:
                sys.stdout.write(buf_line)
            buffer = []
            sys.stdout.write(line)
            brace_count += line.count('{') - line.count('}')

            # メソッド定義が1行で完結する場合の処理
            if brace_count <= 0 and line.rstrip().endswith(';'):
                break
            continue

        # メソッド内の処理
        if in_method:
            sys.stdout.write(line)
            brace_count += line.count('{') - line.count('}')

            # メソッド終了を検出
            if brace_count <= 0:
                break
        else:
            # メソッド外はバッファをクリア (属性以外)
            if not re.match(r'^\s*\[', line):
                buffer = []

    if not output_started:
        print(f"Error: Test method '{class_name}.{method_name}' not found.", file=sys.stderr)
        sys.exit(1)


def main():
    # Windows で標準出力を UTF-8 に設定
    sys.stdout.reconfigure(encoding='utf-8')
    sys.stderr.reconfigure(encoding='utf-8')

    if len(sys.argv) != 4:
        print("Usage: get_test_code_dotnet.py <file_path> <class_name> <method_name>", file=sys.stderr)
        print("", file=sys.stderr)
        print("Example:", file=sys.stderr)
        print("    python3 get_test_code_dotnet.py CalcLibraryTests.cs CalcLibraryTests Add_ShouldReturnCorrectResult", file=sys.stderr)
        sys.exit(1)

    file_path = sys.argv[1]
    class_name = sys.argv[2]
    method_name = sys.argv[3]

    extract_test_code(file_path, class_name, method_name)


if __name__ == '__main__':
    main()
