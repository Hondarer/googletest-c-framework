"""空白を含む配置先で実際の Google Test バイナリとカバレッジ処理を検証する。"""

import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

# Windows の subprocess は System32 を PATH より先に探すため、名前だけで起動すると
# WSL の bash.exe を選ぶことがある。PATH 上の bash (Git Bash など) を明示して使う。
# see: https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw
BASH = shutil.which("bash") or "bash"


TESTFW = Path(__file__).resolve().parents[1]
MAKEFW = TESTFW.parent / "makefw"


class PathSpacesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="testfw space ")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.root = Path(cls.temp.name)
        cls.leaf = cls.root / "test leaf"
        cls.leaf.mkdir()
        (cls.leaf / "bin").mkdir()
        (cls.root / "tmp space").mkdir()
        cls.script = cls.root / "framework/testfw/bin_internal/exec_test_c_cpp.sh"
        shutil.copytree(TESTFW / "bin_internal", cls.script.parent)
        for directory in ("makefiles", "bin_internal"):
            shutil.copytree(MAKEFW / directory, cls.root / "framework/makefw" / directory)
        (cls.root / ".workspaceRoot").touch()
        source = 'int sample(int value) { return value + 1; }\n'
        (cls.root / "source").mkdir()
        (cls.root / "source/sample.c").write_text(source, encoding="utf-8")
        (cls.leaf / "sample.c").write_text(source, encoding="utf-8")
        (cls.leaf / "subprocedure helper.h").write_text('''#include <gtest/gtest.h>
class EvidenceFixture : public ::testing::Test {
    // [サブ手順 名前=EvidenceFixture.SetUp]
    void SetUp() override {
        EXPECT_TRUE(true); // [確認_正常系] - 初期値が true であること。
    }
    // [サブ手順終了]
    // [サブ手順 名前=EvidenceFixture.TearDown]
    void TearDown() override {
        EXPECT_TRUE(true); // [確認_正常系] - 後処理が完了すること。
    }
    // [サブ手順終了]
protected:
    // [サブ手順 名前=EvidenceFixture.Check]
    void Check() {
        EXPECT_TRUE(true); // [確認_正常系] - 結果が true であること。
    }
    // [サブ手順終了]
};
class EvidenceParams : public EvidenceFixture, public ::testing::WithParamInterface<int> {};
''', encoding="utf-8")
        (cls.leaf / "sampleTest.cc").write_text('''#include <gtest/gtest.h>
#include "subprocedure helper.h"
extern "C" int sample(int value);
TEST(PathSpaces, Pass) {
    // Arrange
    int value = 1;
    // Act
    int actual = sample(value);
    // Assert
    EXPECT_EQ(actual, 2);
}
TEST(PathSpaces, Fail) {
    EXPECT_EQ(sample(1), 3);
}
class Repeats : public ::testing::TestWithParam<int> {};
TEST_P(Repeats, Uniform) {
    // Arrange
    int value = GetParam();
    ASSERT_GT(value, 0); // [状態確認] - 正の値であること。
    // Assert
    for (int i = 0; i < 3; ++i) {
        EXPECT_GT(value, 0); // [確認_正常系 回数=PARAM*3] - 正の値であること。
    }
}
TEST_P(Repeats, Branch) {
    // Arrange
    int value = GetParam();
    // Assert
    if (value < 3) {
        EXPECT_LT(value, 3); // [確認_正常系 回数=1+1] - 3 未満であること。
    } else {
        EXPECT_EQ(value, 3); // [確認_異常系] - 3 であること。
    }
}
INSTANTIATE_TEST_SUITE_P(A, Repeats, ::testing::Values(1, 2));
INSTANTIATE_TEST_SUITE_P(B, Repeats, ::testing::Values(3));
TEST(InvalidComment, Case) {
    EXPECT_TRUE(true); // [確認_正常系 回数=1+] - true であること。
}

// [サブ手順参照 名前=EvidenceFixture.SetUp]
TEST_F(EvidenceFixture, ReportsSubprocedures) {
    for (int i = 0; i < 2; ++i) {
        Check(); // [サブ手順参照 名前=EvidenceFixture.Check 回数=2]
    }
}
// [サブ手順参照 名前=EvidenceFixture.TearDown]

// [サブ手順参照 名前=EvidenceFixture.SetUp 回数=PARAM]
TEST_P(EvidenceParams, ReportsSubprocedures) {
    for (int i = 0; i < 2; ++i) {
        Check(); // [サブ手順参照 名前=EvidenceFixture.Check 回数=PARAM*2]
    }
}
// [サブ手順参照 名前=EvidenceFixture.TearDown 回数=PARAM]

INSTANTIATE_TEST_SUITE_P(Records, EvidenceParams, ::testing::Values(1, 2));
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
''', encoding="utf-8")
        cls.env = dict(os.environ, TMPDIR=(cls.root / "tmp space").as_posix())
        for key in ("TEST_SRCS", "ADD_SRCS", "MAKEFW_TEST_LIBS", "WORKSPACE_DIR"):
            cls.env.pop(key, None)
        cls.env["MAKEFW_HOME"] = (cls.root / "framework/makefw").as_posix()
        cls.env["TESTFW_HOME"] = (cls.root / "framework/testfw").as_posix()
        if os.name == "nt":
            libs = TESTFW / "gtest/lib/windows_x64/md"
            command = [
                "cl", "/nologo", "/EHsc", "/MD", "/Od", "/Zi", "/std:c++17",
                "/I" + str(TESTFW / "gtest/include"), "sampleTest.cc", "sample.c",
                "/Febin/test leaf.exe", "/link", "/DEBUG", "/INCREMENTAL:NO",
                "/SUBSYSTEM:CONSOLE", str(libs / "gtest.lib"),
            ]
        else:
            (cls.leaf / "obj").mkdir()
            subprocess.run(
                ["gcc", "--coverage", "-c", "sample.c", "-o", "obj/sample.o"],
                cwd=cls.leaf, check=True,
            )
            # 古い glibc 向けの配布物は新しい Linux でもリンクできる。
            # ディレクトリ名の辞書順では el10 が el8 より先になるため、世代を数値で比較する。
            libraries = sorted(
                TESTFW.glob("gtest/lib/linux_*/libgtest.a"),
                key=lambda path: int(re.search(r"linux_el(\d+)_", path.parent.name).group(1)),
            )
            if not libraries:
                raise RuntimeError("Linux Google Test library not found")
            command = [
                "g++", "--coverage", "-std=c++17", "-pthread",
                "-I" + str(TESTFW / "gtest/include"), "-I" + str(TESTFW / "gtest"),
                "sampleTest.cc", "obj/sample.o", str(libraries[0]), "-o", "bin/test leaf",
            ]
        result = subprocess.run(
            command, cwd=cls.leaf, env=cls.env, stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT, encoding="utf-8", errors="replace", timeout=120,
        )
        if result.returncode:
            raise RuntimeError(result.stdout)

    def run_tests(self, test_filter, coverage=False, source_path=None):
        env = dict(
            self.env, GTEST_FILTER=test_filter, MAKEFW_TEST_FORCE="1",
            TEST_SRCS="../source/sample.c" if coverage else "", ADD_SRCS="",
        )
        if source_path is not None:
            env["TEST_SRCS"] = source_path
        return subprocess.run(
            [BASH, str(self.script)], cwd=self.leaf, env=env,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            encoding="utf-8", errors="replace", timeout=120,
        )

    def test_success_without_coverage(self):
        result = self.run_tests("PathSpaces.Pass")
        self.assertEqual(result.returncode, 0, result.stdout)
        log = (self.leaf / "results/PathSpaces.Pass/results.log").read_text(encoding="utf-8")
        self.assertIn("TEST(PathSpaces, Pass)", log)
        self.assertIn("// Arrange", log)
        self.assertIn("[       OK ] PathSpaces.Pass", log)
        self.assertNotIn("No such file or directory", result.stdout)
        self.assertNotIn("cannot open", result.stdout)

    def test_failure_is_reported(self):
        result = self.run_tests("PathSpaces.Fail")
        self.assertEqual(result.returncode, 1, result.stdout)
        log = (self.leaf / "results/PathSpaces.Fail/results.log").read_text(encoding="utf-8")
        self.assertIn("[  FAILED  ] PathSpaces.Fail", log)
        self.assertFalse((self.leaf / "test.stamp").exists())

    def test_parameter_records_have_one_definition_summary(self):
        result = self.run_tests("*/Repeats.*/*")
        self.assertEqual(result.returncode, 0, result.stdout)
        log = (self.leaf / "results/Repeats.Uniform/results.log").read_text(encoding="utf-8")
        self.assertEqual(log.count("### 確認内容"), 1)
        self.assertIn("### 確認内容 (正常系:9)", log)
        self.assertIn("PARAM=3", log)
        for name in ("Repeats.Uniform/A/0", "Repeats.Uniform/A/1", "Repeats.Uniform/B/0"):
            record = (self.leaf / "results" / name / "results.log").read_text(encoding="utf-8")
            self.assertNotIn("### 確認内容", record)
        branch = (self.leaf / "results/Repeats.Branch/results.log").read_text(encoding="utf-8")
        self.assertIn("### 確認内容 (正常系:2, 異常系:1)", branch)

    def test_fixture_and_header_subprocedures_are_included(self):
        result = self.run_tests("EvidenceFixture.ReportsSubprocedures")
        self.assertEqual(result.returncode, 0, result.stdout)
        log = (self.leaf / "results/EvidenceFixture.ReportsSubprocedures/results.log").read_text(encoding="utf-8")
        self.assertIn("確認内容 (正常系:4)", log)
        self.assertEqual(log.count("// サブ手順: EvidenceFixture.Check\n"), 1)
        self.assertIn("subprocedure helper.h:", log)
        self.assertIn("\nvoid Check()", log)

    def test_parameter_subprocedures_are_aggregated_once(self):
        result = self.run_tests("Records/EvidenceParams.ReportsSubprocedures/*")
        self.assertEqual(result.returncode, 0, result.stdout)
        log = (self.leaf / "results/EvidenceParams.ReportsSubprocedures/results.log").read_text(encoding="utf-8")
        self.assertIn("確認内容 (正常系:8)", log)
        self.assertEqual(log.count("### 確認内容"), 1)
        self.assertEqual(log.count("// サブ手順: EvidenceFixture.Check\n"), 1)
        for n in (0, 1):
            record = (self.leaf / f"results/EvidenceParams.ReportsSubprocedures/Records/{n}/results.log").read_text(encoding="utf-8")
            self.assertNotIn("### 確認内容", record)

    def test_partial_parameter_filter_leaves_counts_unevaluated(self):
        result = self.run_tests("A/Repeats.Uniform/0")
        self.assertEqual(result.returncode, 0, result.stdout)
        log = (self.leaf / "results/Repeats.Uniform/results.log").read_text(encoding="utf-8")
        self.assertIn("### 確認内容 (未評価)", log)
        self.assertNotIn("正常系:9", log)
        self.assertIn("[       OK ] A/Repeats.Uniform/0", log)
        self.assertNotIn("[       OK ] B/Repeats.Uniform/0", log)

    def test_shiftjis_source_and_evidence(self):
        settings = self.root / ".vscode/settings.json"
        settings.parent.mkdir(exist_ok=True)
        source = self.leaf / "sampleTest.cc"
        original = source.read_bytes()
        header = self.leaf / "subprocedure helper.h"
        original_header = header.read_bytes()
        try:
            settings.write_text('{"files.encoding": "shiftjis"}', encoding="utf-8")
            source.write_bytes(original.decode("utf-8").encode("cp932"))
            header.write_bytes(original_header.decode("utf-8").encode("cp932"))
            result = self.run_tests("*/Repeats.Uniform/*")
            self.assertEqual(result.returncode, 0, result.stdout)
            log = (self.leaf / "results/Repeats.Uniform/results.log").read_text(encoding="cp932")
            self.assertIn("### 確認内容 (正常系:9)", log)
            self.assertIn("正の値であること。", log)
        finally:
            source.write_bytes(original)
            header.write_bytes(original_header)
            settings.unlink()

    def test_summary_changes_invalidate_stamp(self):
        source = self.leaf / "sampleTest.cc"
        original = source.read_text(encoding="utf-8")
        summary_script = self.script.parent / "test_summary.py"
        original_script = summary_script.read_text(encoding="utf-8")
        env = dict(self.env, TEST_SRCS="", ADD_SRCS="")
        env.pop("GTEST_FILTER", None)
        env.pop("MAKEFW_TEST_FORCE", None)
        # テスト本体の失敗は別テストで検証し、ここでは成功する定義だけを展開する。
        wrapper = self.leaf / "bin/only pass.sh"
        binary = "test leaf.exe" if os.name == "nt" else "test leaf"
        wrapper.write_text('#!/bin/bash\nexec "./bin/' + binary + '" --gtest_filter=PathSpaces.Pass "$@"\n', encoding="utf-8")
        wrapper.chmod(0o755)
        # 実行スクリプトが使うバイナリ名はカレント ディレクトリで決まるため、
        # テスト用にコピーしたスクリプトだけでラッパーを指定する。
        script_text = self.script.read_text(encoding="utf-8")
        try:
            self.script.write_text(script_text.replace("TEST_BINARY=bin/${PWD##*/}", "TEST_BINARY='bin/only pass.sh'"), encoding="utf-8")
            source.write_text(original.replace("回数=1+]", "回数=1]"), encoding="utf-8")
            def run():
                return subprocess.run([BASH, str(self.script)], cwd=self.leaf, env=env,
                                      stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                      encoding="utf-8", errors="replace", timeout=60)
            first = run()
            self.assertEqual(first.returncode, 0, first.stdout)
            second = run()
            self.assertIn("Skipping test", second.stdout)
            summary_script.write_text(original_script + "\n", encoding="utf-8")
            third = run()
            self.assertEqual(third.returncode, 0, third.stdout)
            self.assertNotIn("Skipping test", third.stdout)
            header = self.leaf / "subprocedure helper.h"
            original_header = header.read_text(encoding="utf-8")
            try:
                header.write_text(original_header + "\n", encoding="utf-8")
                fourth = run()
                self.assertEqual(fourth.returncode, 0, fourth.stdout)
                self.assertNotIn("Skipping test", fourth.stdout)
            finally:
                header.write_text(original_header, encoding="utf-8")
        finally:
            self.script.write_text(script_text, encoding="utf-8")
            summary_script.write_text(original_script, encoding="utf-8")
            source.write_text(original, encoding="utf-8")

    def test_invalid_comment_fails_runner(self):
        result = self.run_tests("InvalidComment.Case")
        self.assertEqual(result.returncode, 1, result.stdout)
        log = (self.leaf / "results/InvalidComment.Case/results.log").read_text(encoding="utf-8")
        self.assertIn("InvalidComment.Case:抽出コード:", log)
        self.assertNotIn("[       OK ] InvalidComment.Case", log)
        self.assertFalse((self.leaf / "test.stamp").exists())

    def test_coverage_is_saved(self):
        result = self.run_tests("PathSpaces.Pass", coverage=True)
        self.assertEqual(result.returncode, 0, result.stdout)
        output = self.leaf / "results/all_tests/coverage.xml"
        self.assertTrue(output.exists(), result.stdout)
        self.assertIn("sample.c", output.read_text(encoding="utf-8"))
        self.assertTrue((self.leaf / "results/PathSpaces.Pass/sample.c.gcov.txt").exists(), result.stdout)
        self.assertNotIn("No such file or directory", result.stdout)
        self.assertNotIn("cannot open", result.stdout)

    def test_absolute_source_with_spaces(self):
        result = self.run_tests(
            "PathSpaces.Pass", source_path=(self.root / "source/sample.c").as_posix()
        )
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertTrue((self.leaf / "results/all_tests/coverage.xml").exists(), result.stdout)
        self.assertIn("source/sample.c", result.stdout)
        self.assertNotIn("Failed to calculate MD5", result.stdout)

    def test_sources_collected_from_child(self):
        child = self.leaf / "child"
        child.mkdir()
        self.addCleanup(shutil.rmtree, child)
        with open(child / "makefile", "w", encoding="utf-8", newline="\n") as handle:
            handle.write((self.root / "framework/makefw/makefiles/__template.mk").read_text(encoding="utf-8"))
        (child / "makelocal.mk").write_text(
            "TEST_SRCS := $(WORKSPACE_DIR)/source/sample.c\n", encoding="utf-8"
        )
        result = self.run_tests("PathSpaces.Pass")
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertTrue((self.leaf / "results/all_tests/coverage.xml").exists(), result.stdout)
        self.assertIn("source/sample.c", result.stdout)


if __name__ == "__main__":
    unittest.main()
