"""期待確認の回数式、タグ、レコード数とサマリー生成を検証する。"""

from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "bin_internal"))
from test_summary import CountExpression, SummaryError, render_summary
from gtest_summary_groups import definition_id, group_tests
from parse_trx_results import parse_trx, TRX_NS

HERE = Path(__file__).resolve().parents[1] / "bin_internal"


class CountExpressionTest(unittest.TestCase):
    def test_precedence_parentheses_and_whitespace(self):
        for expression, expected in [
            ("2+3*4", 14), ("(2+3)*4", 20),
            (" PARAM * (2 + 3) * 2 + 1 ", 41), ("0", 0),
            ("PARAM*2*3+4+5", 33), ("0002", 2),
            ("(" + "1+" * 1200 + "1)", 1201),
        ]:
            with self.subTest(expression=expression[:50]):
                self.assertEqual(CountExpression(expression).evaluate(4), expected)

    def test_invalid_expressions(self):
        for expression in ["", "-1", "+1", "1-1", "1/2", "1.0", "2**3", "PARAMETER",
                           "param", "1 2", "PARAM PARAM", "1+", "*1", "(1+2", "1)", "()",
                           "１", "__import__('os')", "PARAM*2 回数=3"]:
            with self.subTest(expression=expression):
                with self.assertRaises(SummaryError):
                    CountExpression(expression)

    def test_unknown_param_is_not_assumed_one_or_zero(self):
        with self.assertRaises(SummaryError):
            CountExpression("PARAM*0").evaluate()


class SummaryTest(unittest.TestCase):
    def test_counts_and_state_exclusion(self):
        source = '''TEST(Suite, Test)
{
// Arrange
ASSERT_NE(nullptr, p); // [状態確認] - 準備できること。
// Pre-Assert
EXPECT_CALL(mock, Foo()).Times(3); // [Pre-Assert確認_正常系] - 3 回呼ばれること。
EXPECT_CALL(mock, Bar()).Times(0); // [Pre-Assert確認_異常系] - 呼ばれないこと。
// Assert
EXPECT_TRUE(ok); // [確認_正常系 回数=2+3*4] - 値 [0] が一致すること。
EXPECT_FALSE(error); // [確認_異常系 回数=0] - エラーがないこと。
}
'''
        result = render_summary(source)
        self.assertIn("### 確認内容 (正常系:15, 異常系:1)", result)
        self.assertIn("- (準備できること。)", result)
        self.assertIn("値 [0] が一致すること。〔14 回〕", result)

    def test_param_is_explicit_and_not_a_global_multiplier(self):
        source = '''[Theory]
[InlineData(1)]
[InlineData(2)]
public void Test(int value) {
// [確認_正常系] - 全体で 1 回確認すること。
// [確認_正常系 回数=1] - 全体で 1 回確認すること。
// [確認_正常系 回数=PARAM*(2+3)] - 各レコードで確認すること。
// [確認_異常系 回数=2+1] - 分岐で確認すること。
}
'''
        result = render_summary(source, param_count=4)
        self.assertIn("### 確認内容 (正常系:22, 異常系:3)", result)
        self.assertIn("〔20 回: PARAM*(2+3)、PARAM=4〕", result)

    def test_no_source_based_param_inference(self):
        with self.assertRaisesRegex(SummaryError, "レコード数"):
            render_summary('[Theory]\n[InlineData(1)]\n// [確認_正常系 回数=PARAM] - 一致すること。')
        with self.assertRaisesRegex(SummaryError, "通常テスト"):
            render_summary('// [確認_正常系 回数=PARAM] - 一致すること。', param_count=4)

    def test_cycles_and_total(self):
        result = render_summary('''// Arrange
// [確認_正常系 回数=2] - 一致すること。
// Act_2
// [確認_異常系 回数=3] - 失敗すること。
// Assert
// [確認_正常系] - 一致すること。
// Cleanup
''')
        self.assertIn("確認内容_1 (正常系:2)", result)
        self.assertIn("確認内容_2 (正常系:1, 異常系:3)", result)
        self.assertIn("確認件数合計 (正常系:3, 異常系:3)", result)

    def test_partial_execution_does_not_publish_full_counts(self):
        result = render_summary('// [確認_正常系 回数=PARAM*2+1] - 一致すること。', True, 5, True)
        self.assertIn("確認内容 (未評価)", result)
        self.assertIn("〔未評価: PARAM*2+1〕", result)
        self.assertNotIn("正常系:11", result)
        with self.assertRaises(SummaryError):
            render_summary('// [確認_正常系 回数=1+] - 一致すること。', True, 5, True)

    def test_strings_and_block_comments_are_not_tags(self):
        source = '''TEST(Suite, Case) {
const char *s = "// [確認_正常系 回数=500] - 偽。";
const char *r = R"tag(// [確認_正常系 回数=500] - 偽。
// [確認_正常系 回数=500] - 偽。
)tag";
/* // [確認_正常系 回数=500] - 偽。 */
EXPECT_EQ(1, 1); // [確認_正常系] - 一致すること。
}
'''
        self.assertIn("確認内容 (正常系:1)", render_summary(source))
        source = '''[Fact]
public void Test() {
var s = @"// [確認_正常系 回数=500] - 偽。";
var r = """
// [確認_正常系 回数=500] - 偽。
""";
Assert.True(true); // [確認_正常系] - 一致すること。
}
'''
        self.assertIn("確認内容 (正常系:1)", render_summary(source))

    def test_multiple_state_tags(self):
        result = render_summary('// [状態] - 準備する。[状態確認] - 準備できること。')
        self.assertIn("- 準備する。", result)
        self.assertIn("- (準備できること。)", result)
        self.assertIn("確認内容 (0)", result)

    def test_bad_tags_fail_with_location(self):
        for tag in ["[確認]", "[Pre-Assert確認]", "[確認_未知]", "[確認_正常系 回数=]",
                    "[確認_正常系 回数=1 回数=2]", "[確認_正常系 回数=1+]", "[確認_正常系"]:
            with self.subTest(tag=tag):
                with self.assertRaisesRegex(SummaryError, "Suite.Case:抽出コード:2"):
                    render_summary('TEST(Suite, Case)\n// ' + tag + ' - 一致すること。', test_id="Suite.Case")
        with self.assertRaises(SummaryError):
            render_summary('// [確認_正常系] 説明文だけ。')

    # 既定文字コードへの依存を Linux でも検出する。
    @patch("subprocess._text_encoding", return_value="cp1252")
    def test_cli_error_is_nonzero_without_partial_summary(self, _default_encoding):
        for script in ["insert_summary_c_cpp.py", "insert_summary_dotnet.py"]:
            result = subprocess.run([sys.executable, str(HERE / script), "--test-id", "Suite.Case"],
                                    input='// [確認_正常系 回数=1+] - 一致すること。',
                                    text=True, encoding="utf-8", capture_output=True)
            self.assertEqual(result.returncode, 1)
            self.assertEqual(result.stdout, "")
            self.assertIn("Suite.Case:抽出コード:1", result.stderr)

    @patch("subprocess._text_encoding", return_value="cp1252")
    def test_languages_use_the_same_summary(self, _default_encoding):
        source = '// [確認_正常系 回数=2*(3+4)] - 一致すること。\n'
        summaries = []
        for script in ["insert_summary_c_cpp.py", "insert_summary_dotnet.py"]:
            result = subprocess.run([sys.executable, str(HERE / script), "--summary-only"],
                                    input=source, text=True, encoding="utf-8", capture_output=True, check=True)
            summaries.append(result.stdout)
        self.assertEqual(summaries[0], summaries[1])


class RecordCountTest(unittest.TestCase):
    def test_multiple_prefixes_belong_to_one_definition(self):
        names = ["A/Suite.Case/0", "A/Suite.Case/1", "B/Suite.Case/Named"]
        groups = group_tests(names + ["Plain.Case"], names[:1])
        self.assertEqual(groups, {"Suite.Case": {"all": names, "selected": names[:1]}})
        self.assertEqual(definition_id("TypedSuite/0.Case"), "TypedSuite/0.Case")

    def trx_output(self, definitions, results, with_counts=True):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "results.trx"
            path.write_text(f'<TestRun xmlns="{TRX_NS}"><TestDefinitions>{definitions}</TestDefinitions>'
                            f'<Results>{results}</Results></TestRun>', encoding="utf-8")
            output = StringIO()
            with redirect_stdout(output):
                parse_trx(path, with_counts)
            return output.getvalue()

    def test_trx_counts_rows_with_shared_test_id(self):
        definitions = '<UnitTest id="x"><TestMethod className="NS.Suite" name="Case" /></UnitTest>'
        results = ''.join(f'<UnitTestResult testId="x" executionId="e{i}" testName="Case(value: {i})" outcome="Passed" />' for i in range(4))
        self.assertEqual(self.trx_output(definitions, results), "Suite.Case\tPassed\t4\tComplete\n")
        self.assertEqual(self.trx_output(definitions, results, False), "Suite.Case\tPassed\n")

    def test_nested_parent_and_duplicate_execution_are_not_double_counted(self):
        definitions = '<UnitTest id="x"><TestMethod className="Suite" name="Case" /></UnitTest>'
        row = '<UnitTestResult testId="x" executionId="e1" dataRowInfo="0" outcome="Passed" />'
        results = f'<UnitTestResult testId="x" outcome="Passed"><InnerResults>{row}</InnerResults></UnitTestResult>{row}'
        self.assertEqual(self.trx_output(definitions, results), "Suite.Case\tPassed\t1\tComplete\n")

    def test_undiscovered_theory_is_not_assumed_one_record(self):
        definitions = '<UnitTest id="x"><TestMethod className="Suite" name="Case" /></UnitTest>'
        results = '<UnitTestResult testId="x" testName="Case" outcome="Passed" />'
        self.assertEqual(self.trx_output(definitions, results), "Suite.Case\tPassed\t-\tComplete\n")

    def test_skipped_rows_are_partial(self):
        definitions = '<UnitTest id="x"><TestMethod className="Suite" name="Case" /></UnitTest>'
        results = '<UnitTestResult testId="x" testName="Case(a: 1)" outcome="Passed" />' \
                  '<UnitTestResult testId="x" testName="Case(a: 2)" outcome="NotExecuted" />'
        self.assertEqual(self.trx_output(definitions, results), "Suite.Case\tPassed\t1\tPartial\n")


if __name__ == "__main__":
    unittest.main()
