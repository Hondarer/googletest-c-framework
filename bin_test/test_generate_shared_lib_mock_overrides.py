#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""shared library mock override 生成器の API 表処理を検証する。"""

import importlib.util
import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


SCRIPT_PATH = Path(__file__).parents[1] / "bin_internal" / "generate_shared_lib_mock_overrides.py"
SPEC = importlib.util.spec_from_file_location("generate_shared_lib_mock_overrides", SCRIPT_PATH)
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class GenerateSharedLibMockOverridesTest(unittest.TestCase):
    def test_generates_zero_argument_and_callback_api(self):
        source = """\
MOCK_X_RET(int, version, x_app_version, (), (), (const char *caller_file, int caller_line, const char *caller_function), (caller_file, caller_line, caller_function), (_, _, _))
MOCK_X_RET(int, call, x_call, (void (*callback)(int value), int value), (callback, value), (const char *caller_file, int caller_line, const char *caller_function, void (*callback)(int value), int value), (caller_file, caller_line, caller_function, callback, value), (_, _, _, _, _))
"""
        entries = MODULE.parse_api_table(source)

        generated = MODULE.generate(entries, "MOCK_X")

        self.assertIn("extern int mock_x_app_version(const char *caller_file, int caller_line, const char *caller_function);", generated)
        self.assertIn("#define x_app_version() mock_x_app_version(__FILE__, __LINE__, __func__)", generated)
        self.assertIn("mock_x_call(__FILE__, __LINE__, __func__, mock_arg0, mock_arg1)", generated)

    def test_accepts_conditionally_compiled_entry(self):
        entries = MODULE.parse_api_table("#ifdef _WIN32\nMOCK_X_RET(int, win, x_win, (), (), (const char *f, int l, const char *fn), (f, l, fn), (_, _, _))\n#endif\n")

        self.assertEqual("x_win", entries[0][3])
        self.assertIn("#ifdef _WIN32\nextern int mock_x_win", MODULE.generate(entries, "MOCK_X"))

    def test_generates_separate_variadic_wrapper(self):
        ordinary = MODULE.parse_api_table(
            "MOCK_X_RET(int, fixed, x_fixed, (), (), (const char *f, int l, const char *fn), (f, l, fn), (_, _, _))"
        )
        variadic = MODULE.parse_variadic_api_table(
            "MOCK_VARIADIC(int, format, x_format, (const char *format), (format), (const char *caller_file, int caller_line, const char *caller_function, const char *format, va_list args), (caller_file, caller_line, caller_function, format, args))"
        )

        generated = MODULE.generate(ordinary, "MOCK_X", variadic)

        self.assertIn("mock_x_format_variadic(const char *caller_file, int caller_line, const char *caller_function, const char *format, ...)", generated)
        self.assertIn("#define x_format(...) mock_x_format_variadic(__FILE__, __LINE__, __func__, __VA_ARGS__)", generated)

    def test_rejects_duplicate_api_names(self):
        source = "MOCK_X_RET(int, one, x_same, (), (), (const char *f, int l, const char *fn), (f, l, fn), (_, _, _))\nMOCK_X_RET(int, two, x_same, (), (), (const char *f, int l, const char *fn), (f, l, fn), (_, _, _))"

        with self.assertRaisesRegex(ValueError, "duplicate public"):
            MODULE.parse_api_table(source)

    def test_rejects_invalid_identifier_and_field_count(self):
        with self.assertRaisesRegex(ValueError, "invalid API identifier"):
            MODULE.parse_api_table("MOCK_X_RET(int, original, bad-name, (), (), (const char *f, int l, const char *fn), (f, l, fn), (_, _, _))")
        with self.assertRaisesRegex(ValueError, "expected 8"):
            MODULE.parse_api_table("MOCK_X_RET(int, original, x_original, (), (), ())")

    def test_cli_does_not_rewrite_identical_output(self):
        entries = MODULE.parse_api_table("MOCK_X_RET(int, version, x_version, (), (), (const char *f, int l, const char *fn), (f, l, fn), (_, _, _))")
        content = MODULE.generate(entries, "MOCK_X")
        with tempfile.TemporaryDirectory() as temporary_directory:
            output = Path(temporary_directory) / "nested" / "output.h"
            output.parent.mkdir()
            output.write_text(content, encoding="utf-8")
            original_mtime = output.stat().st_mtime_ns
            self.assertFalse(MODULE.write_if_changed(output, content))
            self.assertEqual(original_mtime, output.stat().st_mtime_ns)

    def test_generated_macro_forwards_call_site_expressions(self):
        source = """\
MOCK_X_RET(int, call, x_call, (int table_name), (table_name), (const char *caller_file, int caller_line, const char *caller_function, int table_name), (caller_file, caller_line, caller_function, table_name), (_, _, _, _))
"""
        source += "#ifdef MOCK_UNSUPPORTED_PLATFORM\nMOCK_X_RET(int, win, x_win, (windows_only_type v), (v), (windows_only_type v), (v), (_))\n#endif\n"
        entries = MODULE.parse_api_table(source)
        variadic = MODULE.parse_variadic_api_table(
            "MOCK_VARIADIC(int, format, x_format, (const char *format), (format), (const char *caller_file, int caller_line, const char *caller_function, const char *format, va_list args), (caller_file, caller_line, caller_function, format, args))"
        )
        generated = MODULE.generate(entries, "MOCK_X", variadic)

        with tempfile.TemporaryDirectory() as temporary_directory:
            temporary_path = Path(temporary_directory)
            (temporary_path / "mock_x_overrides.h").write_text(generated, encoding="utf-8")
            (temporary_path / "public_header.h").write_text(
                "#ifndef PUBLIC_HEADER_H\n#define PUBLIC_HEADER_H\n#define x_call(value) value\n#define x_format(format) format\n#endif\n",
                encoding="utf-8",
            )
            (temporary_path / "override.h").write_text(
                '#include "public_header.h"\n#define MOCK_X_INJECT_CALLER\n#include "mock_x_overrides.h"\n',
                encoding="utf-8",
            )
            compile_source = """\
#include "override.h"
int value(void) { int caller_expression = 17; return x_call(caller_expression) + x_format("%d", 42) + x_format("literal"); }
"""
            source_path = temporary_path / "probe.c"
            source_path.write_text(compile_source, encoding="utf-8")
            compiler = shutil.which("cl" if os.name == "nt" else "cc")
            self.assertIsNotNone(compiler, "The required C compiler is unavailable")
            if os.name == "nt":
                command = [compiler, "/nologo", "/WX", "/Zs", "/TC", "/std:c11",
                           "/I" + temporary_directory, str(source_path)]
            else:
                command = [compiler, "-Werror", "-I", temporary_directory,
                           "-x", "c", "-fsyntax-only", str(source_path)]
            result = subprocess.run(
                command,
                text=True,
                encoding="utf-8",
                capture_output=True,
                check=False,
            )

        self.assertEqual(0, result.returncode, result.stderr)


if __name__ == "__main__":
    unittest.main()
