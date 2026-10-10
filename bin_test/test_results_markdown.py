"""結果 Markdown の共通処理、構造、確認件数を検証する。"""

import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'bin_internal'))
import results_markdown as md
from test_subprocedures import SourceIndex
from test_summary import render_summary


def assert_structure(case, path):
    data = Path(path).read_bytes()
    case.assertFalse(data.startswith(b'\xef\xbb\xbf'), str(path))
    case.assertNotIn(b'\r', data, str(path))
    case.assertTrue(data.endswith(b'\n') and not data.endswith(b'\n\n'), str(path))
    text = data.decode('utf-8')
    marker = None
    h1 = 0
    empty = False
    for line in text.splitlines():
        match = re.match(r'^(`{3,})(\w*)$', line)
        if match:
            if marker is None:
                marker = match[1]
            elif match[1] == marker and not match[2]:
                marker = None
            empty = False
        elif marker is None:
            h1 += line.startswith('# ')
            case.assertFalse(empty and not line, str(path))
            empty = not line
    case.assertIsNone(marker, str(path))
    case.assertEqual(h1, 1, str(path))
    case.assertTrue(text.startswith('# '), str(path))
    case.assertNotIn('\x1b', text, str(path))


def assert_results(case, root):
    root = Path(root)
    case.assertFalse(list(root.rglob('*.log')))
    case.assertFalse(list(root.rglob('*.gcov.txt')))
    for path in root.rglob('*.md'):
        assert_structure(case, path)
        from urllib.parse import unquote
        # 出力の各節にある相対リンクが、生成したファイルへ到達することを確認する。
        content = path.read_text(encoding='utf-8')
        for target in re.findall(r'\]\(([^)]+)\)', content):
            if target.endswith('results.md'):
                case.assertTrue((path.parent / unquote(target)).exists(), str(path) + ': ' + target)
    summary = (root / 'all_tests/summary.md').read_text(encoding='utf-8')
    # 件数表と、リンク先の同じ解析結果が一致することを確認する。
    normal, abnormal, unknown = 0, 0, 0
    body = summary.split('## 確認件数\n\n', 1)[1].split('\n## ', 1)[0]
    for target, a, b, total in re.findall(r'^\| \[.+?\]\((.*?)\) \| (.*?) \| (.*?) \| (.*?) \|$', body, re.M):
        from urllib.parse import unquote
        result = (root / 'all_tests' / unquote(target)).read_text(encoding='utf-8')
        if a == '未評価':
            unknown += 1
            case.assertEqual([a, b, total], ['未評価'] * 3)
            case.assertTrue('未評価' in result or '[!CAUTION]' in result)
            continue
        headings = re.findall(r'^### 確認件数合計 \((.*?)\)$', result, re.M)
        if not headings:
            headings = re.findall(r'^### 確認内容(?:_\d+)? \((.*?)\)$', result, re.M)
        expected = [sum(int(v) for h in headings for v in re.findall(category + r':(\d+)', h)) for category in ['正常系', '異常系']]
        case.assertEqual([int(a), int(b)], expected, target)
        case.assertEqual(int(total), int(a) + int(b))
        normal += int(a)
        abnormal += int(b)
    if '| 合計 |' in body:
        case.assertIn(f'| 合計 | {normal} | {abnormal} | {normal + abnormal} |', body)
    if unknown:
        case.assertIn(f'未評価のテスト定義 {unknown} 件', body)


class ResultsMarkdownTest(unittest.TestCase):
    def test_fence_is_longer_than_any_run_and_empty_is_none(self):
        self.assertEqual(md.fence(''), 'なし')
        self.assertEqual(md.fence(' \n\n'), 'なし')
        self.assertEqual(md.fence('x``` y`````\n\n', 'cpp'), '``````cpp\nx``` y`````\n``````')
        self.assertEqual(md.fence('```', fixed=True), '```text\n```\n```')

    def test_inline_cells_and_links(self):
        self.assertEqual(md.inline('a``b'), '``` a``b ```')
        self.assertEqual(md.cell('a|b'), r'a\|b')
        self.assertEqual(md.table(['項目'], [['値']], '表題'), '| 項目 |\n| --- |\n| 値 |\n\nTable: 表題')
        self.assertEqual(md.link('結果', '../a b/#x?%/results.md'), '[結果](../a%20b/%23x%3F%25/results.md)')

    def test_names_are_plain_text_with_markdown_escapes(self):
        # 単語内の _ はそのまま残し、記法として解釈される文字だけをエスケープする。
        self.assertEqual(md.plain('test_static_access.test'), 'test_static_access.test')
        self.assertEqual(md.plain('Multi/Param.Test/0'), 'Multi/Param.Test/0')
        self.assertEqual(md.plain('_a*b[c]<d>`e`\\'), r'\_a\*b\[c\]\<d\>\`e\`\\')
        self.assertEqual(md.cell(md.plain('p|q')), r'p\|q')

    def test_summary_counts_are_returned_from_same_analysis(self):
        counts = {}
        summary = render_summary('// [確認_正常系 回数=2*3] - 一致すること。\n// Act_2\n// [確認_異常系] - 失敗すること。', counts=counts)
        self.assertEqual(counts, {'正常系': 6, '異常系': 1})
        self.assertIn('確認件数合計 (正常系:6, 異常系:1)', summary)
        self.assertIn('### 状態_1\n\nなし', summary)
        self.assertNotIn('----', summary)
        self.assertNotIn('確認件数合計 (正常系:6, 異常系:1)\n\nなし', summary)
        counts = {}
        self.assertEqual(render_summary('TEST(S, T) {}', counts=counts), '')
        self.assertEqual(counts, {'正常系': 0, '異常系': 0})

    def test_individual_failure_and_console(self):
        result = md.individual('S.T', 'FAILED', '', '\x1b[31mError: bad\x1b[0m', error='抽出できません')
        self.assertIn('> [!CAUTION]\n> 抽出できません', result)
        self.assertNotIn('\x1b', result)
        preserved = md.individual("S.T", "FAILED", "## テスト コード\n\n```cpp\nvoid test() {}\n```", "OK", error="集計エラー")
        self.assertIn("void test() {}", preserved)
        tricky = md.individual("S.T", "PASSED", "## テスト コード\n\n```cpp\n## 実行結果\n\nsource\n```", "runtime```")
        self.assertEqual(md.execution_output(tricky), "runtime```")
        console = md.individual('S.T', 'PASSED', '', '\x1b[32mOK\x1b[0m', console=True)
        self.assertNotIn('- 判定:', console)
        self.assertIn('\x1b[32m', console)
        self.assertIn('```text', console)

    def test_definitions_and_record_links(self):
        records = [{'name': 'P/S.T/0', 'status': 'WARNING', 'path': 'P/0/results.md', 'output': '```\nOK'},
                   {'name': 'P/S.T/1', 'status': 'FAILED', 'path': 'P/1/results.md', 'output': ''}]
        result = md.definition('S.T', '## テスト項目\n\nなし\n\n## テスト コード\n\nなし', records, 3)
        self.assertIn('- 総合判定: FAILED', result)
        self.assertIn('- 実行レコード数: 2 / 3 (WARNING: 1, FAILED: 1)', result)
        self.assertEqual(result.count('## テスト コード'), 1)
        self.assertIn('[results.md](P/0/results.md)', result)
        self.assertIn('````text', result)
        record = md.individual('S.T/P/0', 'WARNING', '## テスト コード\n\nなし', 'OK', definition='../../results.md')
        self.assertIn('[テスト定義の結果](../../results.md)', record)
        record = md.individual("S.T/0", "PASSED", "## テスト コード\n\nなし", "OK", definition="auto")
        self.assertIn("[テスト定義の結果](../results.md)", record)

    def test_gcov_body_and_path_are_preserved(self):
        body = '        -:    0:Source:/workspace/app/a b.c\n        1:    1:```\n'
        result = md.gcov(body, '/workspace')
        self.assertIn('# a b.c のカバレッジ', result)
        self.assertIn('- ソース: app/a b.c', result)
        self.assertIn('````text\n' + body + '````', result)

    def test_group_cli_reads_shiftjis_list_and_writes_after_records(self):
        script = Path(md.__file__).parent / "gtest_summary_groups.py"
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "sample.cc").write_text('TEST_P(S, T) {\n// [確認_正常系] - 日本語の確認。\n}\nINSTANTIATE_TEST_SUITE_P(P, S, Values(1));\n', encoding="cp932")
            names = root / "list"
            names.write_bytes("P/S.T/0 # 日本語\n".encode("cp932"))
            manifest = root / "manifest"
            subprocess.run([sys.executable, str(script), "prepare", str(manifest), "--full-list", str(names), "--selected-list", str(names), "--encoding", "shiftjis"], cwd=root, check=True, capture_output=True)
            self.assertFalse((root / "results/S.T/results.md").exists())
            state = json.loads(manifest.read_text(encoding="utf-8"))
            md.write(root / "results/S.T/P/0/results.md", md.individual("S.T/P/0", "PASSED", state["S.T"]["evidence"], "OK", definition="auto"))
            subprocess.run([sys.executable, str(script), "finish", str(manifest), "--counts-dir", str(root / "counts"), "--encoding", "shiftjis"], cwd=root, check=True, capture_output=True)
            self.assertIn("日本語", (root / "results/S.T/results.md").read_text(encoding="utf-8"))
            self.assertEqual(json.loads((root / "counts/S.T.json").read_text(encoding="utf-8")), {"正常系": 1, "異常系": 0})
            assert_structure(self, root / "results/S.T/results.md")

    def test_summary_counts_unknown_order_and_early_error(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'S.T.json').write_text(json.dumps({'正常系': 7, '異常系': 2}), encoding='utf-8')
            journal = 'Test start on today.\nS.T\tPASSED\t# a|b\nP.T/X/0\tPASSED\nS.T\tPASSED\nTotal tests\t3\nPassed\t3\nWarning(s)\t0\nFailed\t0\n@test\tS.T\n[ WARNING ] warning\nError: gcovr\ndetail 1\ndetail 2\n'
            result = md.summary('leaf', journal, root, filter_value='P.*')
            self.assertIn('| 合計 | 7 | 2 | 9 |', result)
            self.assertIn('未評価のテスト定義 1 件', result)
            self.assertEqual(result.count('[S.T](../S.T/results.md) | 7'), 1)
            self.assertIn(r'`# a\|b`', result)
            self.assertIn('> [!WARNING]', result)
            self.assertIn('> [!CAUTION]', result)
            self.assertIn('> ```text', result)
            path = root / 'summary.md'
            md.write(path, result)
            assert_structure(self, path)
            early = md.summary('leaf', 'Test start on today.\nError: missing binary\n', root)
            self.assertIn('## 警告とエラー', early)
            self.assertIn('## 確認件数\n\nなし', early)

    def test_encoding_conversion_and_invalid_bytes_via_cli(self):
        script = Path(md.__file__)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            evidence = root / 'evidence'
            evidence.write_text('## テスト項目\n\nなし\n\n## テスト コード\n\nなし', encoding='utf-8')
            for encoding, body in [('shiftjis', '日本語```'.encode('cp932') + b'\x81'), ('utf8bom', b'\xef\xbb\xbf' + '日本語'.encode('utf-8') + b'\xff')]:
                runtime = root / 'runtime'
                runtime.write_bytes(body)
                output = root / 'results.md'
                subprocess.run([sys.executable, str(script), 'individual', '--test-id', 'S.T', '--input', str(runtime), '--evidence', str(evidence), '--encoding', encoding, '--output', str(output)], check=True, capture_output=True)
                text = output.read_text(encoding='utf-8')
                self.assertIn('日本語', text)
                self.assertIn('\ufffd', text)
                self.assertNotIn('\ufeff', text)
                self.assertIn('- 判定: PASSED', text)
                assert_structure(self, output)

    def test_gcov_directory_cli_matches_individual_conversion(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "gcov"
            source.mkdir()
            bodies = ["-:0:Source:/workspace/app/日本語.c\n1:1:日本語```\n", "-:0:Source:/workspace/app/b.c\n0:1:other\n"]
            for i, body in enumerate(bodies):
                (source / f"{i}.gcov").write_bytes(body.encode("cp932") + b"\x81")
            output = root / "output"
            subprocess.run([sys.executable, md.__file__, "gcov", "--input", str(source),
                            "--output", str(output), "--encoding", "shiftjis", "--workspace", "/workspace"], check=True, capture_output=True)
            for path in source.glob("*.gcov"):
                result = output / (path.name + ".md")
                self.assertEqual(result.read_text(encoding="utf-8"), md.gcov(md.read(path, "shiftjis", True), "/workspace"))
                assert_structure(self, result)

    def test_one_invocation_writes_file_and_encoded_colored_console(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            runtime = root / "runtime"
            body = "日本語```\nテストの実行に成功しました。\n"
            runtime.write_text(body, encoding="utf-8")
            output, console = root / "results.md", root / "console"
            subprocess.run([sys.executable, md.__file__, "individual", "--test-id", "S.T",
                            "--input", str(runtime), "--output", str(output), "--console-output", str(console),
                            "--console-encoding", "shiftjis", "--dotnet-color"], check=True, capture_output=True)
            self.assertEqual(output.read_text(encoding="utf-8"), md.individual("S.T", "PASSED", "", body))
            text = console.read_text(encoding="cp932")
            expected = md.individual("S.T", "PASSED", "", body, console=True).replace(
                "テストの実行に成功しました。", "\x1b[32mテストの実行に成功しました。\x1b[0m")
            self.assertEqual(text, expected)
            assert_structure(self, output)

    def test_evidence_cli_also_writes_encoded_console_prefix(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "sample.cc"
            source.write_text("TEST(S, T) {\n// [確認_正常系] - 日本語の確認。\n}\n", encoding="cp932")
            prefix = root / "prefix"
            result = subprocess.run([sys.executable, str(Path(md.__file__).parent / "test_subprocedures.py"),
                                     "--test-id", "S.T", "--source", str(source), "--encoding", "shiftjis",
                                     "--console-prefix-output", str(prefix), "--binary", "bin/test"],
                                    cwd=root, check=True, capture_output=True)
            evidence = result.stdout.decode("utf-8")
            expected = md.individual("S.T", "PASSED", evidence, "", binary="bin/test", console=True)
            expected = expected.split("## 実行結果\n", 1)[0] + "## 実行結果\n\n```text\n"
            self.assertEqual(prefix.read_text(encoding="cp932"), expected)
            self.assertIn("日本語", expected)

    def test_shiftjis_journal_keeps_utf8_diagnostics_in_order(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "日本語の一時ディレクトリ"
            root.mkdir()
            diagnostic = root / "error"
            diagnostic.write_text("[  FAILED  ] 日本語のエビデンスエラー\n", encoding="utf-8")
            journal = root / "journal"
            journal.write_bytes(("S.T\tFAILED\t日本語\n@test\tS.T\n@utf8\t" + diagnostic.name +
                                 "\n[ WARNING ] 日本語の警告\n").encode("cp932"))
            output = root / "summary.md"
            subprocess.run([sys.executable, md.__file__, "summary", "--input", str(journal),
                            "--counts-dir", str(root), "--encoding", "shiftjis", "--output", str(output)], check=True, capture_output=True)
            text = output.read_text(encoding="utf-8")
            self.assertIn("日本語のエビデンスエラー", text)
            self.assertIn("日本語の警告", text)
            self.assertNotIn("\ufffd", text)
            self.assertLess(text.index("エビデンスエラー"), text.index("日本語の警告"))
            assert_structure(self, output)

    def test_subprocedure_report_counts_and_structure(self):
        counts = {}
        source = '// [サブ手順 名前=Check]\nvoid Check() {\n// [確認_正常系 回数=2] - 一致すること。\n}\n// [サブ手順終了]\nTEST(S, T) {\n Check(); // [サブ手順参照 名前=Check 回数=3]\n}\n'
        evidence = SourceIndex({'sample.cc': source}).report('S.T', counts=counts)
        self.assertEqual(counts, {'正常系': 6, '異常系': 0})
        self.assertIn('```cpp', evidence)
        self.assertEqual(evidence.count('// サブ手順: Check'), 1)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'results.md'
            md.write(path, md.individual('S.T', 'PASSED', evidence, 'OK'))
            assert_structure(self, path)

class DotnetMarkdownRouteTest(unittest.TestCase):
    """VSTest の通信環境に依存せず、TRX から Markdown までを検証する。"""

    def run_route(self, encoding='utf8', partial=False, list_error=False):
        import os
        import shutil
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        root = Path(self.temp.name)
        scripts = root / 'framework/testfw/bin_internal'
        shutil.copytree(Path(md.__file__).parent, scripts)
        helper = root / 'framework/makefw/bin_internal'
        helper.mkdir(parents=True)
        shutil.copy2(Path(md.__file__).parents[2] / 'makefw/bin_internal/get_files_lang.sh', helper)
        (root / '.workspaceRoot').touch()
        (root / '.vscode').mkdir()
        (root / '.vscode/settings.json').write_text(json.dumps({'files.encoding': encoding}), encoding='utf-8')
        leaf = root / 'test leaf'
        leaf.mkdir()
        source = '''using Xunit;
public class ExampleTests {
[Theory]
[InlineData(1)]
[InlineData(2)]
public void Case(int value) {
// [状態確認] - 準備できること。
// [確認_正常系 回数=PARAM*3] - 値が一致すること。
// [確認_異常系] - エラーがないこと。
}
[Fact]
public void Plain() {}
}
'''
        (leaf / 'ExampleTests.cs').write_text(source, encoding='cp932' if encoding == 'shiftjis' else 'utf-8')
        fake = root / 'dotnet fixture.py'
        fake.write_text('''#!/usr/bin/env python3
import os, sys
from pathlib import Path
if '--list-tests' in sys.argv:
    if os.environ.get('LIST_ERROR'):
        raise SystemExit(2)
    print('    ExampleTests.Case(value: 1)')
    print('    ExampleTests.Case(value: 2)')
    print('    ExampleTests.Plain')
else:
    directory = Path(sys.argv[sys.argv.index('--results-directory') + 1])
    directory.mkdir(parents=True, exist_ok=True)
    outcome = 'NotExecuted' if os.environ.get('PARTIAL') else 'Passed'
    xml = f'<TestRun xmlns="http://microsoft.com/schemas/VisualStudio/TeamTest/2010"><TestDefinitions><UnitTest id="x"><TestMethod className="ExampleTests" name="Case" /></UnitTest><UnitTest id="y"><TestMethod className="ExampleTests" name="Plain" /></UnitTest></TestDefinitions><Results><UnitTestResult testId="x" executionId="a" testName="Case(value: 1)" outcome="Passed" /><UnitTestResult testId="x" executionId="b" testName="Case(value: 2)" outcome="{outcome}" /><UnitTestResult testId="y" executionId="c" testName="Plain" outcome="Passed" /></Results></TestRun>'
    (directory / 'results.trx').write_text(xml, encoding='utf-8')
    encoding = 'cp932' if os.environ.get('ENCODING') == 'shiftjis' else 'utf-8'
    body = '  Passed ExampleTests.Case(value: 1) 日本語```\\n  Passed ExampleTests.Plain\\n'
    sys.stdout.buffer.write(body.encode(encoding) + (b'\\x81' if encoding == 'cp932' else b'\\xff') + b'\\n')
''', encoding='utf-8')
        fake.chmod(0o755)
        env = dict(os.environ, DOTNET=str(fake), CONFIG='Debug', ENCODING=encoding)
        if partial:
            env['PARTIAL'] = '1'
        if list_error:
            env['LIST_ERROR'] = '1'
        result = subprocess.run([shutil.which('bash') or 'bash', str(scripts / 'exec_test_dotnet.sh')], cwd=leaf, env=env, capture_output=True, encoding='utf-8', errors='replace', timeout=30)
        return result, leaf / 'results'

    def test_dotnet_full_counts_and_dynamic_fences(self):
        result, root = self.run_route()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        assert_results(self, root)
        self.assertIn('| 合計 | 6 | 1 | 7 |', (root / 'all_tests/summary.md').read_text(encoding='utf-8'))
        individual = (root / 'ExampleTests.Case/results.md').read_text(encoding='utf-8')
        self.assertIn('日本語', individual)
        self.assertIn('````text', individual)
        self.assertNotIn('- 判定:', result.stdout)
        self.assertNotIn('## 確認件数\n', result.stdout)

    def test_dotnet_shiftjis_and_partial_counts(self):
        result, root = self.run_route('shiftjis', True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        assert_results(self, root)
        self.assertIn('未評価のテスト定義 1 件', (root / 'all_tests/summary.md').read_text(encoding='utf-8'))
        self.assertIn('日本語', (root / 'ExampleTests.Case/results.md').read_text(encoding='utf-8'))

    def test_dotnet_early_error_writes_summary(self):
        result, root = self.run_route(list_error=True)
        self.assertEqual(result.returncode, 2)
        assert_results(self, root)
        self.assertIn('> [!CAUTION]', (root / 'all_tests/summary.md').read_text(encoding='utf-8'))


class CoverageAndColorTest(unittest.TestCase):
    def test_fixed_width_and_markdown_have_same_values(self):
        import contextlib
        import io
        from cobertura2gcovr import print_report
        data = [('a|b.c', 4, 3, [2], 2, 1), ('none.c', 0, 0, [], 0, 0)]
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            print_report(data)
        fixed = output.getvalue()
        markdown = md.coverage_rows(data)
        self.assertIn('Code Coverage Report', fixed)
        self.assertIn(r'| a\|b.c | 4 | 3 | 75% | 2 | 50% | 2 |', markdown)
        self.assertIn('| TOTAL | 4 | 3 | 75% | 2 | 50% |  |', markdown)
        self.assertIn('| none.c | 0 | 0 | 0% | - | - |  |', markdown)

    def test_generated_gcov_preserves_source_bytes_before_markdown(self):
        from cobertura2gcov import generate_gcov
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "sample.c"
            body = "// 日本語\n".encode("cp932") + b"// \x81\n"
            source.write_bytes(body)
            path = generate_gcov(str(source), {"lines": {1: 1, 2: 0}}, root, "cp932")
            self.assertIn(b"// \x81\n", Path(path).read_bytes())
            result = md.gcov(md.read(path, "shiftjis", True), root)
            self.assertIn("日本語", result)
            self.assertIn("\ufffd", result)

    def test_windows_color_filter_encoding_preserves_markdown(self):
        script = Path(md.__file__).parent / 'add_gtest_color.py'
        source = '# `日本語`\n\n```text\n[ RUN      ] S.T 日本語\n```\n'
        result = subprocess.run([sys.executable, str(script), '--encoding', 'shiftjis'], input=source.encode('cp932') + b'\x81\n', capture_output=True, check=True)
        self.assertTrue(result.stdout.endswith(b'\x81\n'))
        colored = result.stdout[:-2].decode('cp932')
        self.assertIn('# `日本語`\n\n```text\n', colored)
        self.assertIn('\x1b[0;32m[ RUN      ]\x1b[0m S.T 日本語', colored)
        self.assertTrue(colored.endswith('```\n'))


if __name__ == '__main__':
    unittest.main()
