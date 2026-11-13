#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""API 表から C の mock 宣言と呼び出し元注入マクロを生成する。"""

import argparse
import os
import re
import sys
import tempfile
from pathlib import Path


IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def split_arguments(text):
    """入れ子の括弧を考慮して API 表マクロの引数を分割する。"""
    result = []
    start = 0
    depth = 0
    for index, char in enumerate(text):
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth < 0:
                raise ValueError("unbalanced parentheses")
        elif char == "," and depth == 0:
            result.append(text[start:index].strip())
            start = index + 1
    if depth != 0:
        raise ValueError("unbalanced parentheses")
    result.append(text[start:].strip())
    return result



class ApiEntries(list):
    """API rows and the preprocessor branches that expose each row."""
    def __init__(self):
        super().__init__()
        self.conditions = {}


def conditions_at(source, offset):
    stack = []
    for line in source[:offset].splitlines():
        match = re.match(r"\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b", line)
        if not match:
            continue
        directive = match.group(1)
        if directive in ("if", "ifdef", "ifndef"):
            stack.append([line.strip()])
        elif directive in ("elif", "else"):
            if not stack:
                raise ValueError("preprocessor branch without an opening condition")
            stack[-1].append(line.strip())
        elif stack:
            stack.pop()
        else:
            raise ValueError("unmatched preprocessor endif")
    return [tuple(group) for group in stack]


def preserve_conditions(content, entries):
    conditions = {api: groups for api, groups in getattr(entries, "conditions", {}).items() if groups}
    output = []
    for line in content.splitlines():
        selected = None
        for api, groups in conditions.items():
            if re.search(r"\bmock_" + re.escape(api) + r"(?:_variadic)?\(", line) or line.startswith("#define " + api + "("):
                selected = groups
                break
        if selected:
            for group in selected:
                output.extend(group)
        output.append(line)
        if selected:
            output.extend("#endif" for _ in selected)
    return "\n".join(output) + "\n"


def parse_api_table(source):
    """RET/VOID 表の8列を読み取り、元名と公開名を検証する。"""
    entries = ApiEntries()
    index = 0
    while True:
        match = re.search(r"\bMOCK_[A-Z0-9_]+_(RET|VOID)\s*\(", source[index:])
        if match is None:
            break
        macro_start = index + match.start()
        open_paren = index + match.end() - 1
        depth = 1
        cursor = open_paren + 1
        while cursor < len(source) and depth:
            if source[cursor] == "(":
                depth += 1
            elif source[cursor] == ")":
                depth -= 1
            cursor += 1
        if depth:
            raise ValueError("unterminated API table entry")
        fields = split_arguments(source[open_paren + 1:cursor - 1])
        if len(fields) != 8:
            raise ValueError(f"expected 8 API table fields, got {len(fields)} at offset {macro_start}")
        return_type, original, api, parameters, arguments, mock_parameters, mock_arguments, matchers = fields
        if not IDENTIFIER.fullmatch(original) or not IDENTIFIER.fullmatch(api):
            raise ValueError(f"invalid API identifier: {original!r}, {api!r}")
        entries.conditions[api] = conditions_at(source, macro_start)
        entries.append((match.group(1), return_type, original, api, parameters, arguments,
                        mock_parameters, mock_arguments, matchers))
        index = cursor
    if not entries:
        raise ValueError("API table has no entries")
    for field_index, label in ((2, "original"), (3, "public")):
        names = [entry[field_index] for entry in entries]
        if len(names) != len(set(names)):
            raise ValueError(f"duplicate {label} API name")
    return entries


def parse_variadic_api_table(source):
    """固定引数情報だけを持つ専用可変長 API 表を読む。"""
    entries = ApiEntries()
    index = 0
    while True:
        match = re.search(r"\bMOCK_VARIADIC\s*\(", source[index:])
        if match is None:
            break
        start = index + match.start()
        open_paren = index + match.end() - 1
        depth = 1
        cursor = open_paren + 1
        while cursor < len(source) and depth:
            if source[cursor] == "(":
                depth += 1
            elif source[cursor] == ")":
                depth -= 1
            cursor += 1
        if depth:
            raise ValueError("unterminated variadic API table entry")
        fields = split_arguments(source[open_paren + 1:cursor - 1])
        if len(fields) != 7:
            raise ValueError(f"expected 7 variadic API table fields, got {len(fields)} at offset {start}")
        return_type, original, api, parameters, arguments, mock_parameters, mock_arguments = fields
        if not IDENTIFIER.fullmatch(original) or not IDENTIFIER.fullmatch(api):
            raise ValueError(f"invalid variadic API identifier: {original!r}, {api!r}")
        entries.conditions[api] = conditions_at(source, start)
        entries.append((return_type, original, api, parameters, arguments, mock_parameters, mock_arguments))
        index = cursor
    names = [entry[2] for entry in entries]
    if len(names) != len(set(names)):
        raise ValueError("duplicate variadic public API name")
    return entries


def generate(entries, prefix, variadic_entries=()):
    """8列 API 表から C 宣言と呼び出し元位置付き関数形式マクロを出力する。"""
    if not IDENTIFIER.fullmatch(prefix):
        raise ValueError(f"invalid macro prefix: {prefix!r}")
    lines = ["/* Generated by generate_shared_lib_mock_overrides.py. */", f"#ifndef {prefix}_MOCK_DECLARATIONS", f"#define {prefix}_MOCK_DECLARATIONS", ""]
    lines.extend(["#ifdef __cplusplus", 'extern "C" {', "#endif"])
    for _, return_type, _, api, _, _, mock_parameters, _, _ in entries:
        params = mock_parameters[1:-1].strip()
        lines.append(f"extern {return_type} mock_{api}({params});")
    for return_type, _, api, parameters, _, _, _ in variadic_entries:
        params = parameters[1:-1].strip()
        if params:
            params = ", " + params
        lines.append(f"extern {return_type} mock_{api}_variadic(const char *caller_file, int caller_line, const char *caller_function{params}, ...);")
    lines.append("")
    lines.extend(["#ifdef __cplusplus", "}", "#endif", "", "#endif", ""])
    for _, _, _, api, _, _, _, _, _ in entries:
        lines.extend([f"#ifdef {api}", f"#undef {api}", "#endif"])
    for _, _, api, _, _, _, _ in variadic_entries:
        lines.extend([f"#ifdef {api}", f"#undef {api}", "#endif"])
    lines.extend(["", f"#ifdef {prefix}_INJECT_CALLER"])
    for _, _, _, api, _, arguments, _, _, _ in entries:
        real_arguments = split_arguments(arguments[1:-1]) if arguments[1:-1].strip() else []
        macro_arguments = [f"mock_arg{index}" for index in range(len(real_arguments))]
        formal_arguments = ", ".join(macro_arguments)
        forwarded_arguments = ", ".join(macro_arguments)
        if forwarded_arguments:
            forwarded_arguments = ", " + forwarded_arguments
        lines.append(f"#define {api}({formal_arguments}) mock_{api}(__FILE__, __LINE__, __func__{forwarded_arguments})")
    for _, _, api, _, arguments, _, _ in variadic_entries:
        real_arguments = split_arguments(arguments[1:-1]) if arguments[1:-1].strip() else []
        macro_arguments = [f"mock_arg{index}" for index in range(len(real_arguments))]
        formal_arguments = "..."
        forwarded_arguments = "__VA_ARGS__"
        lines.append(f"#define {api}({formal_arguments}) mock_{api}_variadic(__FILE__, __LINE__, __func__, {forwarded_arguments})")
    lines.extend(["#endif", ""])
    content = preserve_conditions("\n".join(lines), entries)
    return preserve_conditions(content, variadic_entries)


def write_if_changed(output, content):
    """出力内容が変わった場合だけファイルを書き換える。"""
    if output.exists() and output.read_text(encoding="utf-8") == content:
        return False
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary_path = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="w", encoding="utf-8", newline="\n", dir=output.parent, delete=False
        ) as temporary_file:
            temporary_file.write(content)
            temporary_path = Path(temporary_file.name)
        os.replace(temporary_path, output)
    finally:
        if temporary_path is not None and temporary_path.exists():
            temporary_path.unlink()
    return True


def main():
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("api_table", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--macro-prefix", required=True)
    parser.add_argument("--variadic-api-table", type=Path)
    args = parser.parse_args()
    try:
        entries = parse_api_table(args.api_table.read_text(encoding="utf-8"))
        variadic_entries = []
        if args.variadic_api_table is not None:
            variadic_entries = parse_variadic_api_table(args.variadic_api_table.read_text(encoding="utf-8"))
        content = generate(entries, args.macro_prefix, variadic_entries)
    except (OSError, ValueError) as error:
        print(f"generate_shared_lib_mock_overrides.py: {error}", file=sys.stderr)
        return 1
    write_if_changed(args.output, content)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
