"""サブ手順の件数、前後の所属、抜粋と解析エラーを検証する。"""

from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "bin_internal"))
from test_subprocedures import SourceIndex, dedent_source, platform_lines
from test_summary import SummaryError, render_summary


def definition(name, body, indent=""):
    return f'{indent}// [サブ手順 名前={name}]\n{body}\n{indent}// [サブ手順終了]\n'


def test(body="", before="", after="", name="Suite.Case", macro="TEST_F"):
    suite, case = name.split(".")
    return f'{before}{macro}({suite}, {case})\n{{\n{body}\n}}\n{after}'


class SubprocedureTest(unittest.TestCase):
    def test_unresolved_reference_cannot_be_silently_ignored(self):
        with self.assertRaises(SummaryError):
            render_summary('// [サブ手順参照 名前=Missing]\n')

    def test_inline_definition_counts_only_through_reference(self):
        source = test(definition("Region", 'EXPECT_TRUE(ok); // [確認_正常系] - 結果が真であること。')
                      + '// [サブ手順参照 名前=Region 回数=3]')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容 (正常系:3)", report)

    def test_transitive_counts_and_source_once_per_definition(self):
        source = definition("B", 'void b() {\nEXPECT_TRUE(ok); // [確認_正常系 回数=2] - B が一致すること。\n}')
        source += definition("A", 'void a() {\nb(); // [サブ手順参照 名前=B 回数=3]\n}')
        source += test('a(); // [サブ手順参照 名前=A 回数=4]\na(); // [サブ手順参照 名前=A]\n')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容 (正常系:30)", report)
        self.assertEqual(report.count("// サブ手順: A\n"), 1)
        self.assertEqual(report.count("// サブ手順: B\n"), 1)
        # 経路は入れ子の階層で示し、回数は参照ごとの値を各階層に付ける。
        self.assertIn("- サブ手順 A にて以下を確認する。〔4 回〕\n    - サブ手順 B にて以下を確認する。〔3 回〕\n        - B が一致すること。〔2 回〕\n"
                      "- サブ手順 A にて以下を確認する。\n    - サブ手順 B にて以下を確認する。〔3 回〕\n", report)
        self.assertNotIn("参照回数", report)
        self.assertLess(report.index("// サブ手順: A\n"), report.index("// サブ手順: B\n"))

    def test_before_after_order_and_last_cycle(self):
        source = "".join(definition(n, f'void f() {{\nEXPECT_TRUE(ok); // [確認_正常系] - {n}。\n}}') for n in ["Before", "After"])
        source += test('// Assert_2\nEXPECT_TRUE(ok); // [確認_正常系] - 本体。',
                       before='// [サブ手順参照 名前=Before]\n', after='// [サブ手順参照 名前=After]\n')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容_1 (正常系:1)", report)
        self.assertIn("確認内容_2 (正常系:2)", report)
        self.assertLess(report.index("- Before。"), report.index("- 本体。"))
        self.assertLess(report.index("- 本体。"), report.index("- After。"))

    def test_internal_cycles_do_not_change_parent_cycles(self):
        source = definition("A", 'void a() {\n// Assert_3\nEXPECT_TRUE(ok); // [確認_正常系] - A。\n}')
        source += test('a(); // [サブ手順参照 名前=A]\n// Assert_2\nEXPECT_TRUE(ok); // [確認_異常系] - 本体。')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容_1 (正常系:1)", report)
        self.assertNotIn("確認内容_3", report)

    def test_explicit_param_total_and_partial(self):
        source = definition("A", 'void a() {\nEXPECT_TRUE(ok); // [確認_正常系] - A。\n}')
        source += test('a(); // [サブ手順参照 回数=PARAM * (2+3) 名前=A]', macro="TEST_P")
        index = SourceIndex({"sample.cc": source})
        self.assertIn("確認内容 (正常系:20)", index.report("Suite.Case", param_count=4))
        self.assertIn("- サブ手順 A にて以下を確認する。〔20 回: PARAM*(2+3)、PARAM=4〕\n    - A。\n", index.report("Suite.Case", param_count=4))
        self.assertIn("確認内容 (未評価)", index.report("Suite.Case", param_count=4, partial=True))
        self.assertIn("- サブ手順 A にて以下を確認する。〔未評価: PARAM*(2+3)〕", index.report("Suite.Case", param_count=4, partial=True))
        self.assertIn("// サブ手順: A", index.report("Suite.Case", param_count=4, partial=True))
        self.assertNotIn("## テスト項目", index.report("Suite.Case", code_only=True))

    def test_reference_category_override_and_nested_precedence(self):
        source = definition("B", '// [確認_正常系] - B。\n// [状態確認] - 準備。')
        source += definition("A", '// [サブ手順参照 名前=B]\n// [サブ手順参照 名前=B 区分=正常系]')
        source += test('// [サブ手順参照 名前=A 回数=3 区分=異常系]')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容 (正常系:3, 異常系:3)", report)
        self.assertIn("準備。", report)
        with self.assertRaisesRegex(SummaryError, "区分"):
            SourceIndex({"sample.cc": definition("B", '') + test('// [サブ手順参照 名前=B 区分=未知]')})

    def test_inactive_fixture_definition_is_resolved_without_count(self):
        source = '#if defined(PLATFORM_WINDOWS)\n' + definition("Fixture.SetUp", '// [確認_正常系] - Windows。') + '#endif\n'
        source += test(before='// [サブ手順参照 名前=Fixture.SetUp]\n')
        linux = SourceIndex({"sample.cc": source}, is_windows=False).report("Suite.Case")
        windows = SourceIndex({"sample.cc": source}, is_windows=True).report("Suite.Case")
        self.assertIn("確認内容 (0)", linux)
        self.assertIn("正常系:1", windows)
        self.assertNotIn("Windows。", linux)

    def test_unknown_preprocessor_guards_are_preserved(self):
        source = '#ifndef UNKNOWN\nvoid f() {}\n#endif\n'
        self.assertEqual(''.join(platform_lines(source, False)), source)

    def test_zero_count_still_has_excerpt(self):
        source = definition("A", 'void a() {\nEXPECT_TRUE(ok); // [確認_正常系] - A。\n}') + test('a(); // [サブ手順参照 名前=A 回数=0]')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容 (0)", report)
        self.assertIn("- サブ手順 A にて以下を確認する。〔0 回〕\n    - A。\n", report)
        self.assertIn("// サブ手順: A", report)

    def test_nested_items_follow_phase_order_and_home_sections(self):
        source = definition("Setup", '// [状態] - 初期化する。\n// [状態確認] - 資源があること。')
        source += definition("Inner", '// [状態確認] - 長さが範囲内であること。')
        source += definition("Mock", '// [Pre-Assert手順] - 値を返す。\n// [Pre-Assert確認_正常系] - 呼ばれること。\n'
                                     '// [サブ手順参照 名前=Inner]\n// [Pre-Assert確認_正常系 回数=2] - 引数が一致すること。')
        source += definition("Check", '// [確認_正常系] - 一致すること。')
        source += definition("Down", '// [手順] - 解放する。') + "\n"
        source += test('// Arrange\n// [状態] - 構造体を用意する。\n// Pre-Assert\n// [サブ手順参照 名前=Mock]\n'
                       '// Act\n// [手順] - 編集する。\n// Assert\n// [確認_正常系] - 成功すること。\n'
                       '// [サブ手順参照 名前=Check 回数=3]',
                       before='// [サブ手順参照 名前=Setup]\n', after='// [サブ手順参照 名前=Down]\n')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("""### 状態

- サブ手順 Setup にて以下を準備する。
    - 初期化する。
- 構造体を用意する。

### 手順

- 編集する。
- サブ手順 Mock を実施する。
    - 値を返す。
    - サブ手順 Inner を実施する。
- サブ手順 Down を実施する。
    - 解放する。

### 確認内容 (正常系:7)

- サブ手順 Setup にて以下を確認する。
    - (資源があること。)
- サブ手順 Mock にて以下を確認する。
    - 呼ばれること。
    - サブ手順 Inner にて以下を確認する。
        - (長さが範囲内であること。)
    - 引数が一致すること。〔2 回〕
- 成功すること。
- サブ手順 Check にて以下を確認する。〔3 回〕
    - 一致すること。
""", report)

    def test_parent_without_children_and_cycles_of_before_and_after(self):
        source = definition("Setup", '// [状態確認] - 資源があること。') + definition("Down", '// [状態確認] - 解放したこと。') + "\n"
        source += test('// Act\n// [手順] - 操作する。\n// Act_2\n// [手順] - 再度操作する。',
                       before='// [サブ手順参照 名前=Setup]\n', after='// [サブ手順参照 名前=Down]\n')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("### 状態_1\n\n- サブ手順 Setup を実施する。\n", report)
        self.assertIn("### 確認内容_1 (0)\n\n- サブ手順 Setup にて以下を確認する。\n    - (資源があること。)\n", report)
        self.assertIn("### 状態_2\n\nなし\n", report)
        self.assertIn("### 手順_2\n\n- 再度操作する。\n- サブ手順 Down を実施する。\n", report)
        self.assertIn("### 確認内容_2 (0)\n\n- サブ手順 Down にて以下を確認する。\n    - (解放したこと。)\n", report)
        self.assertEqual(report.count("サブ手順 Down"), 2)

    def test_code_blank_lines_are_squeezed(self):
        source = test('int a;\n#if defined(PLATFORM_WINDOWS)\nint w;\nint x;\n#endif\n\n\nint b;\n// [状態] - 準備する。')
        report = SourceIndex({"sample.cc": source}, is_windows=False).report("Suite.Case")
        code = report.split("## テスト コード", 1)[1]
        self.assertIn("int a;\n\nint b;", code)
        self.assertNotIn("\n\n\n", code)

    def test_state_step_and_state_check_are_included_without_count(self):
        source = definition("A", '// [状態] - 初期値。\n// [手順] - 準備する。\n// [状態確認] - 資源を確保したこと。') + test('a(); // [サブ手順参照 名前=A 回数=3]')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        for text in ["初期値。", "準備する。", "資源を確保したこと。", "確認内容 (0)"]:
            self.assertIn(text, report)

    def test_strings_are_not_markers_and_braces_in_strings_do_not_end_test(self):
        source = test('const char *raw = R"x(}\n// [サブ手順 名前=Fake]\n)x";\nEXPECT_TRUE(ok); // [確認_正常系] - 値 [0]。')
        report = SourceIndex({"sample.cc": source}).report("Suite.Case")
        self.assertIn("確認内容 (正常系:1)", report)
        self.assertIn("値 [0]", report)

    def test_internal_description_keeps_brackets(self):
        source = definition("A", 'EXPECT_TRUE(ok); // [確認_正常系] - 値 [0] が一致すること。') + test('a(); // [サブ手順参照 名前=A]')
        self.assertIn("値 [0] が一致すること。", SourceIndex({"sample.cc": source}).report("Suite.Case"))

    def test_cross_file_definition_and_relative_header_and_symlink_dedup(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            header = root / "helper.h"
            header.write_text(definition("A", 'EXPECT_TRUE(ok); // [確認_正常系] - A。'), encoding="utf-8")
            cpp = root / "test.cc"
            cpp.write_text('#include "helper.h"\n' + test('a(); // [サブ手順参照 名前=A]'), encoding="utf-8")
            alias = root / "alias.cc"
            alias.symlink_to(cpp)
            index = SourceIndex.from_paths([cpp, alias])
            self.assertEqual(len(index.tests["Suite.Case"]), 1)
            self.assertIn("正常系:1", index.report("Suite.Case"))

    def test_dotnet_attributes_and_method_dedent(self):
        source = 'class Suite {\n' + definition("A", '    void A() {\n        Assert.True(ok); // [確認_正常系] - A。\n    }', "    ")
        source += '    // [サブ手順参照 名前=A 回数=PARAM]\n    [Theory]\n    [InlineData(1)]\n    [InlineData(2)]\n    public void Case(int x)\n    {\n        Assert.True(ok); // [確認_正常系 回数=PARAM] - 本体。\n    }\n}\n'
        report = SourceIndex({"sample.cs": source}, "dotnet").report("Suite.Case", param_count=2)
        self.assertIn("正常系:4", report)
        self.assertIn("\npublic void Case", report)
        self.assertIn("\nvoid A()", report)

    def test_dedent_preserves_multiline_literal_and_preprocessor(self):
        source = '    void a() {\n#if defined(PLATFORM_LINUX)\n        const char *s = R"x(\n    payload\n)x";\n        const char *cs = @"\n    content\n";\n#endif\n    }\n'
        actual = dedent_source(source, "    ")
        self.assertTrue(actual.startswith("void a() {\n#if"))
        self.assertIn('R"x(\n    payload\n)x"', actual)
        self.assertIn('@"\n    content\n"', actual)
        self.assertIn("    const char *", actual)

    def test_tabs_keep_relative_indent(self):
        self.assertEqual(dedent_source('\tvoid a() {\n\t\tcall();\n\t}\n', '\t'), 'void a() {\n\tcall();\n}\n')

    def test_platform_branches(self):
        source = definition("A", 'void a() {\n#if defined(PLATFORM_WINDOWS)\nEXPECT_TRUE(ok); // [確認_正常系 回数=2] - Windows。\n#else\nEXPECT_TRUE(ok); // [確認_正常系 回数=3] - Linux。\n#endif\n}') + test('a(); // [サブ手順参照 名前=A]')
        for windows, count in [(True, 2), (False, 3)]:
            with self.subTest(windows=windows):
                self.assertIn(f"正常系:{count}", SourceIndex({"sample.cc": source}, is_windows=windows).report("Suite.Case"))

    def test_adjacent_tests_are_ambiguous_but_blank_separates(self):
        prefix = definition("A", '// [状態] - A。')
        ambiguous = test(after='// [サブ手順参照 名前=A]\n') + test(name="Suite.Other")
        with self.assertRaisesRegex(SummaryError, "曖昧"):
            SourceIndex({"sample.cc": prefix + ambiguous})
        separated = test(after='// [サブ手順参照 名前=A]\n') + "\n" + test(name="Suite.Other")
        self.assertIn("A。", SourceIndex({"sample.cc": prefix + separated}).report("Suite.Case"))

    def test_detached_reference_is_error(self):
        source = definition("A", '// [状態] - A。') + '// [サブ手順参照 名前=A]\n\n' + test()
        with self.assertRaisesRegex(SummaryError, "所属しない"):
            SourceIndex({"sample.cc": source})

    def test_unknown_duplicate_cycle_and_missing_end(self):
        invalid = [
            test('a(); // [サブ手順参照 名前=Missing 回数=0]'),
            definition("A", '') + definition("A", ''),
            definition("A", '// [サブ手順参照 名前=B]') + definition("B", '// [サブ手順参照 名前=A]'),
            '// [サブ手順 名前=A]\nvoid a() {}\n',
            '// [サブ手順終了]\n',
            definition("A", '// [サブ手順 名前=B]\n'),
            definition("A", '// [確認] - 不正。'),
            '// [サブ手順定義 名前=A]\n',
        ]
        for source in invalid:
            with self.subTest(source=source), self.assertRaises(SummaryError):
                SourceIndex({"sample.cc": source})

    def test_invalid_attributes_and_param_inside_definition(self):
        invalid = [
            '名前=A 回数=1 回数=2', '名前=A 位置=前', '名前=A 不明=1',
            '名前=A 名前=B', '名前=A 回数=1+', '名前=',
        ]
        for attrs in invalid:
            with self.subTest(attrs=attrs), self.assertRaises(SummaryError):
                SourceIndex({"sample.cc": definition("A", '') + test('// [サブ手順参照 ' + attrs + ']')})
        for body in ['// [確認_正常系 回数=PARAM] - A。', '// [サブ手順参照 名前=B 回数=PARAM]']:
            with self.subTest(body=body), self.assertRaisesRegex(SummaryError, "PARAM"):
                SourceIndex({"sample.cc": definition("A", body) + definition("B", '')})

    def test_orphan_check_errors_but_state_check_is_allowed(self):
        with self.assertRaisesRegex(SummaryError, "所属しない"):
            SourceIndex({"sample.cc": 'void a() {\nEXPECT_TRUE(ok); // [確認_正常系] - A。\n}\n'})
        SourceIndex({"sample.cc": 'void a() {\nEXPECT_TRUE(ok); // [状態確認] - A。\n}\n'})

    # 既定文字コードへの依存を Linux でも検出する。
    @patch("subprocess._text_encoding", return_value="cp1252")
    def test_cli_failure_has_no_partial_output(self, _default_encoding):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "test.cc"
            path.write_text(test('// [サブ手順参照 名前=Missing]'), encoding="utf-8")
            script = Path(__file__).resolve().parents[1] / 'bin_internal/test_subprocedures.py'
            result = subprocess.run([sys.executable, str(script), '--test-id', 'Suite.Case'], cwd=directory,
                                    text=True, encoding="utf-8", capture_output=True)
            self.assertEqual(result.returncode, 1)
            self.assertEqual(result.stdout, '')
            self.assertIn('test.cc:', result.stderr)


if __name__ == '__main__':
    unittest.main()
