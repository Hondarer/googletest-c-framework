"""承認済みの表示仕様とプラットフォーム条件の回帰テスト。"""

import contextlib
import io
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

HERE = Path(__file__).resolve().parents[1] / 'bin_internal'
sys.path.insert(0, str(HERE))
import results_markdown as md
from parse_trx_results import format_local_time, parse_trx
from test_subprocedures import SourceIndex, platform_lines
from test_summary import render_summary


class ResultsRefinementTest(unittest.TestCase):
    def test_description_survives_nested_setup_expansion(self):
        source = '''// [サブ手順 名前=Inner]
void Inner() {
// [状態] - 内側を準備する。
}
// [サブ手順終了]
// [サブ手順 名前=Fixture.SetUp]
void SetUp() {
// [サブ手順参照 名前=Inner]
}
// [サブ手順終了]

// libcalc のエクスポート シンボル名が API 表と一致することの確認
// [サブ手順参照 名前=Fixture.SetUp]
TEST_F(Fixture, Example) {
// Arrange
}
'''
        report = SourceIndex({'sample.cc': source}).report('Fixture.Example')
        overview = report.split('## テスト コード')[0]
        self.assertIn('libcalc のエクスポート シンボル名が API 表と一致することの確認', overview)
        self.assertIn('内側を準備する。', overview)

    def test_description_is_discarded_at_existing_boundaries(self):
        for boundary in ('', '// [状態] - 別のタグ', 'int value;', '// [手順] - 別のタグ'):
            with self.subTest(boundary=boundary):
                result = render_summary('// 破棄する説明\n' + boundary + '\nTEST(S, T) {\n// [確認_正常系] - 確認。\n}')
                self.assertNotIn('破棄する説明', result)

    def test_empty_sections_in_single_and_multiple_cycles(self):
        for suffix, body in (('', ''), ('_1', '// Act_2\n')):
            report = render_summary('// 説明\nTEST(S, T) {\n' + body + '}')
            self.assertIn(f'### 状態{suffix}\n\n特記すべき状態なし', report)
            self.assertIn(f'### 手順{suffix}\n\nなし', report)
            self.assertIn(f'### 確認{suffix} (0)\n\nなし', report)
            if suffix:
                self.assertIn('### 状態_2\n\n特記すべき状態なし', report)
        self.assertEqual(render_summary('TEST(S, T) {}'), '')

    def test_metadata_and_console_for_individual_error_and_records(self):
        stamp = '2026-10-11 10:00:00 JST'
        for test_id, options in [('S.T', {}), ('S.T/P/0', {'definition': 'auto'}), ('S.Failed', {'error': '抽出失敗'})]:
            with self.subTest(test_id=test_id):
                result = md.individual(test_id, 'PASSED', '', 'OK', binary='bin/test', executed_at=stamp, **options)
                self.assertIn('- 判定: PASSED\n- 実施日時: ' + stamp, result)
                self.assertNotIn('テスト バイナリ:', result)
                console = md.individual(test_id, 'PASSED', '', 'OK', binary='bin/test', console=True, executed_at=stamp, **options)
                self.assertIn('- テスト バイナリ: bin/test', console)
                self.assertNotIn('実施日時:', console)
        result = md.definition('S.T', 'evidence', [dict(name='P/S.T/0', status='PASSED', executed_at=stamp, path='P/0/results.md', output='OK')], 1)
        self.assertIn('- 判定: PASSED\n- 実施日時: ' + stamp + '\n- 詳細:', result)
        self.assertNotIn('実施日時', result.split('## 実行レコード')[0])
        with tempfile.TemporaryDirectory() as directory:
            result = md.summary('leaf', 'Test start on old console date.\n@executed_at\t' + stamp, directory)
        self.assertIn('- 実施日時: ' + stamp, result)
        self.assertNotIn('開始日時', result)
        self.assertNotIn('old console date', result)

    def test_platform_formats_agree_between_body_and_subprocedures(self):
        for windows in (False, True):
            for macro in ('PLATFORM_WINDOWS', '_WIN32', 'PLATFORM_LINUX', '__linux__'):
                defined = (macro in ('PLATFORM_WINDOWS', '_WIN32')) == windows
                for expression, negate in [(f'ifdef {macro}', False), (f'ifndef {macro}', True),
                                           (f'if defined( {macro} )', False), (f'if ! defined ( {macro} )', True)]:
                    with self.subTest(windows=windows, expression=expression):
                        source = f'TEST(S, T) {{\n  # {expression}\n// [状態] - YES\n  # else\n// [状態] - NO\n  # endif\n}}\n'
                        body = subprocess.check_output(['awk', '-v', 'test_id=S.T', '-v', f'is_windows={int(windows)}', '-f', str(HERE / 'get_test_code_c_cpp.awk')], input=source, text=True, encoding='utf-8')
                        sub = ''.join(platform_lines(source, windows))
                        expected, absent = ('YES', 'NO') if defined != negate else ('NO', 'YES')
                        for output in (body, sub):
                            self.assertIn(expected, output)
                            self.assertNotIn(absent, output)
                            self.assertNotIn('#', output)
                        fragment = '// [サブ手順 名前=Setup]\nvoid Setup() {\n' + '\n'.join(source.splitlines()[1:-1]) + '\n}\n// [サブ手順終了]\n\nTEST(S, T) {\n// [サブ手順参照 名前=Setup]\n}\n'
                        report = SourceIndex({'sample.cc': fragment}, is_windows=windows).report('S.T')
                        self.assertIn(expected, report)
                        self.assertNotIn(absent, report)

    def test_unknown_platform_conditions_and_elif(self):
        for expression in ('ifdef WIN32', 'ifndef _WIN64', 'if defined(linux)', 'if defined(LINUX)',
                           'if defined(_WIN32) || defined(__linux__)', 'if defined(_WIN32) // comment',
                           'ifdef _WIN32 /* comment */'):
            source = f'TEST(S, T) {{\n#{expression}\nYES();\n#else\nNO();\n#endif\n}}\n'
            for windows in (0, 1):
                body = subprocess.check_output(['awk', '-v', 'test_id=S.T', '-v', f'is_windows={windows}', '-f', str(HERE / 'get_test_code_c_cpp.awk')], input=source, text=True, encoding='utf-8')
                self.assertEqual(body, ''.join(platform_lines(source, bool(windows))))
        source = 'TEST(S, T) {\n#ifdef _WIN32\nWIN();\n#elif ! defined( _WIN32 )\n#ifndef __linux__\nBAD();\n#else\nLINUX();\n#endif\n#endif\n}\n'
        for windows, expected, absent in ((0, 'LINUX', 'WIN'), (1, 'WIN', 'LINUX')):
            body = subprocess.check_output(['awk', '-v', 'test_id=S.T', '-v', f'is_windows={windows}', '-f', str(HERE / 'get_test_code_c_cpp.awk')], input=source, text=True, encoding='utf-8')
            for output in (body, ''.join(platform_lines(source, bool(windows)))):
                self.assertIn(expected, output)
                self.assertNotIn(absent, output)
                self.assertNotIn('BAD', output)

    def test_trx_start_time_earliest_record_offsets_fraction_and_fallback(self):
        xml = '''<TestRun xmlns="http://microsoft.com/schemas/VisualStudio/TeamTest/2010">
<TestDefinitions><UnitTest id="a"><TestMethod className="Tests" name="Theory" /></UnitTest><UnitTest id="b"><TestMethod className="Tests" name="Fact" /></UnitTest></TestDefinitions>
<Results><UnitTestResult testId="a" executionId="a1" testName="Theory(1)" outcome="Passed" startTime="2026-10-11T10:00:00.900+09:00" />
<UnitTestResult testId="a" executionId="a2" testName="Theory(2)" outcome="Passed" startTime="2026-10-11T00:59:59.100Z" />
<UnitTestResult testId="a" executionId="a3" testName="Theory(3)" outcome="Passed" />
<UnitTestResult testId="b" executionId="b1" outcome="Passed" /></Results></TestRun>'''
        # Git for Windows の date は tz データベースを同梱せず、'Asia/Tokyo' を UTC として扱う。
        # 双方の date が解釈できる POSIX 形式で JST を指定する。
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, TZ='JST-9'):
            path = Path(directory) / 'results.trx'
            path.write_text(xml, encoding='utf-8')
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                parse_trx(path, True, True, '2026-10-11 09:00:00 JST')
            self.assertIn('Tests.Theory\tPassed\t3\tComplete\t2026-10-11 09:59:59 JST', output.getvalue())
            self.assertIn('Tests.Fact\tPassed\t-\tComplete\t2026-10-11 09:00:00 JST', output.getvalue())

    def test_local_time_uses_offset_when_zone_abbreviation_is_blank(self):
        # 日本語の Windows で TZ が未設定のとき、Cygwin 系の date は %Z を空白だけで出力する。
        def fake_date(command, **kwargs):
            if command[-1] == '+%Z':
                return '   \n'
            return '2026-10-11 09:59:59 +0900\n' if command[-1].endswith('%z') else 'unexpected\n'
        with patch('parse_trx_results.subprocess.check_output', side_effect=fake_date):
            self.assertEqual(format_local_time('2026-10-11T00:59:59Z'), '2026-10-11 09:59:59 +0900')
        with tempfile.TemporaryDirectory() as directory:
            fake = Path(directory) / 'date'
            fake.write_text('#!/bin/bash\ncase "${!#}" in\n  +%Z) echo "   " ;;\n  *%z) echo "2026-10-11 09:59:59 +0900" ;;\n  *) echo unexpected ;;\nesac\n', encoding='utf-8', newline='\n')
            fake.chmod(0o755)
            # Windows の bash へは cygpath で変換した一時ディレクトリを PATH の先頭に渡す。
            command = 'PATH="$(cygpath -u "$1" 2>/dev/null || printf %s "$1"):$PATH" bash "$2"'
            output = subprocess.check_output([shutil.which('bash') or 'bash', '-c', command, 'bash', directory, str(HERE / 'local_time.sh')], text=True, encoding='utf-8')
            self.assertEqual(output, '2026-10-11 09:59:59 +0900\n')


if __name__ == '__main__':
    unittest.main()
