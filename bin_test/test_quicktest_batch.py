#!/usr/bin/env python3

from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "bin_internal"))
import quicktest_batch as qb


BATCH_STDOUT = """Note: Google Test filter = *
Running main() from gmock_main.cc
[==========] Running 5 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 5 tests from Sample
[ RUN      ] Sample.Pass
[       OK ] Sample.Pass (3 ms)
[ RUN      ] Sample.Skip
sample.cc:10: Skipped
not supported here

[  SKIPPED ] Sample.Skip (0 ms)
[ RUN      ] Sample.NoNewline
partial output[       OK ] Sample.NoNewline (1 ms)
[ RUN      ] Sample.Fail
sample.cc:20: Failure
Value of: false
[  FAILED  ] Sample.Fail (2 ms)
[ RUN      ] P/Sample.Param/0
[  FAILED  ] P/Sample.Param/0, where GetParam() = 4 (5 ms)
[----------] 5 tests from Sample (11 ms total)

[----------] Global test environment tear-down
[==========] 5 tests from 1 test suite ran. (11 ms total)
[  PASSED  ] 2 tests.
[  SKIPPED ] 1 test, listed below:
[  SKIPPED ] Sample.Skip
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] Sample.Fail
[  FAILED  ] P/Sample.Param/0, where GetParam() = 4

 2 FAILED TESTS

  YOU HAVE 1 DISABLED TEST

"""


def case(status, result, failures=None):
    data = {"status": status, "result": result}
    if failures:
        data["failures"] = failures
    return data


class QuicktestBatchTest(unittest.TestCase):
    def test_stdout_blocks_cover_every_end_kind(self):
        main_line, blocks, outside = qb.parse_batch_stdout(BATCH_STDOUT)
        self.assertEqual(main_line, "Running main() from gmock_main.cc")
        self.assertEqual(blocks["Sample.Pass"][:2], ("OK", "3"))
        self.assertEqual(blocks["Sample.Skip"][:2], ("SKIPPED", "0"))
        self.assertIn("not supported here\n", blocks["Sample.Skip"][2])
        self.assertEqual(blocks["Sample.NoNewline"][:2], ("OK", "1"))
        self.assertIn("partial output[       OK ] Sample.NoNewline (1 ms)\n", blocks["Sample.NoNewline"][2])
        self.assertEqual(blocks["Sample.Fail"][:2], ("FAILED", "2"))
        self.assertEqual(blocks["P/Sample.Param/0"][:2], ("FAILED", "5"))
        self.assertEqual(len(blocks), 5)
        # 末尾の失敗一覧はテスト区間外の診断行として残る。
        self.assertIn("[  FAILED  ] Sample.Fail", outside)

    def test_filter_colors_follow_googletest(self):
        from add_gtest_color import AnsiColor, colorize_line
        # GoogleTest はスキップを緑、無効テストを黄で出力する。
        self.assertEqual(colorize_line("[  SKIPPED ] Sample.Skip (0 ms)"),
                         AnsiColor.GREEN + "[  SKIPPED ]" + AnsiColor.RESET + " Sample.Skip (0 ms)")
        self.assertEqual(colorize_line("[ DISABLED ] Sample.DISABLED_Off"),
                         AnsiColor.YELLOW + "[ DISABLED ]" + AnsiColor.RESET + " Sample.DISABLED_Off")
        # GoogleTest が着色しない見出しは、そのまま出力する。
        for line in ("[  DEATH   ] child output", "[  INFO ] file.cc:1: note"):
            self.assertEqual(colorize_line(line), line)
        # 警告だけは、本フレームワーク独自に黄で着色する。
        self.assertEqual(colorize_line("[WARNING] file.cc:1: note"),
                         AnsiColor.YELLOW + "[WARNING]" + AnsiColor.RESET + " file.cc:1: note")

    def test_unfinished_blocks_are_kept_without_end_kind(self):
        text = "[ RUN      ] Sample.Lost\nnoise\n[ RUN      ] Sample.Crash\nabout to crash\n"
        _, blocks, _ = qb.parse_batch_stdout(text)
        self.assertEqual(blocks["Sample.Lost"], (None, "0", "[ RUN      ] Sample.Lost\nnoise\n"))
        self.assertEqual(blocks["Sample.Crash"], (None, "0", "[ RUN      ] Sample.Crash\nabout to crash\n"))

    def test_outcome_follows_stdout_and_json(self):
        _, blocks, _ = qb.parse_batch_stdout(BATCH_STDOUT)
        self.assertEqual(qb.resolve_outcome("Sample.Pass", blocks, {}), qb.OUTCOME_OK)
        self.assertEqual(qb.resolve_outcome("Sample.Skip", blocks, {}), qb.OUTCOME_SKIPPED)
        self.assertEqual(qb.resolve_outcome("Sample.Fail", blocks, {}), qb.OUTCOME_FAILED)
        failed = {"Sample.Pass": case("RUN", "COMPLETED", [{"failure": "x"}])}
        self.assertEqual(qb.resolve_outcome("Sample.Pass", blocks, failed), qb.OUTCOME_FAILED)

    def test_outcome_of_tests_without_end_line(self):
        blocks = {"Sample.Lost": (None, "0", "[ RUN      ] Sample.Lost\n")}
        self.assertEqual(qb.resolve_outcome("Sample.Lost", blocks, {}), qb.OUTCOME_ABORTED)
        completed = {"Sample.Lost": case("RUN", "COMPLETED")}
        self.assertEqual(qb.resolve_outcome("Sample.Lost", blocks, completed), qb.OUTCOME_OK)
        skipped = {"Sample.Lost": case("RUN", "SKIPPED")}
        self.assertEqual(qb.resolve_outcome("Sample.Lost", blocks, skipped), qb.OUTCOME_SKIPPED)

    def test_outcome_of_tests_without_block(self):
        disabled = {"Sample.DISABLED_Off": case("NOTRUN", "SUPPRESSED")}
        self.assertEqual(qb.resolve_outcome("Sample.DISABLED_Off", {}, disabled), qb.OUTCOME_DISABLED)
        # 異常終了で JSON がない場合は、テスト名から無効テストを判別する。
        self.assertEqual(qb.resolve_outcome("Sample.DISABLED_Off", {}, {}), qb.OUTCOME_DISABLED)
        self.assertEqual(qb.resolve_outcome("P/DISABLED_Sample.Case/0", {}, {}), qb.OUTCOME_DISABLED)
        self.assertEqual(qb.resolve_outcome("Sample.Later", {}, {}), qb.OUTCOME_NOT_RUN)
        self.assertEqual(qb.resolve_outcome("Sample.Later", {}, {"Sample.Later": case("RUN", "COMPLETED")}), qb.OUTCOME_NOT_RUN)

    def test_runtime_text_matches_individual_run_format(self):
        main_line, blocks, _ = qb.parse_batch_stdout(BATCH_STDOUT)

        def text(name, outcome, exit_code=0):
            return qb.build_runtime_text("bin/sample", name, main_line, outcome, blocks.get(name), exit_code)

        passed = text("Sample.Pass", qb.OUTCOME_OK)
        self.assertTrue(passed.startswith("./bin/sample --gtest_filter=Sample.Pass\nRunning main() from gmock_main.cc\n"))
        self.assertIn("[ RUN      ] Sample.Pass\n[       OK ] Sample.Pass (3 ms)\n", passed)
        self.assertTrue(passed.endswith("[  PASSED  ] 1 test.\n"))

        skipped = text("Sample.Skip", qb.OUTCOME_SKIPPED)
        self.assertTrue(skipped.endswith(
            "[  PASSED  ] 0 tests.\n[  SKIPPED ] 1 test, listed below:\n[  SKIPPED ] Sample.Skip\n"))
        self.assertIsNone(qb.WARNING_LINE.search(skipped))

        failed = text("Sample.Fail", qb.OUTCOME_FAILED, 1)
        self.assertIn("Value of: false\n[  FAILED  ] Sample.Fail (2 ms)\n", failed)
        self.assertTrue(failed.endswith("[  FAILED  ] Sample.Fail\n\n 1 FAILED TEST\n"))

        disabled = text("Sample.DISABLED_Off", qb.OUTCOME_DISABLED)
        self.assertIn("[==========] Running 0 tests from 0 test suites.\n", disabled)
        self.assertIn("[  PASSED  ] 0 tests.\n", disabled)
        self.assertIn("  YOU HAVE 1 DISABLED TEST\n", disabled)
        self.assertNotIn("[ RUN      ]", disabled)

        not_run = text("Sample.Later", qb.OUTCOME_NOT_RUN, 139)
        self.assertIn("[  FAILED  ] Not executed", not_run)
        self.assertIn("exit code: 139", not_run)

        crash = ("OK", "0", "[ RUN      ] Sample.Crash\nabout to crash\n")
        aborted = qb.build_runtime_text("bin/sample", "Sample.Crash", "", qb.OUTCOME_ABORTED, crash, 139)
        self.assertIn("about to crash\n\n[  FAILED  ] Terminated abnormally", aborted)


if __name__ == "__main__":
    unittest.main()
