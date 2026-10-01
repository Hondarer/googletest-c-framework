"""空白を含む配置先で実際の Google Test バイナリとカバレッジ処理を検証する。"""

import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest


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
        (cls.leaf / "sampleTest.cc").write_text('''#include <gtest/gtest.h>
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
            ["bash", str(self.script)], cwd=self.leaf, env=env,
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
        (child / "makefile").write_text(
            (self.root / "framework/makefw/makefiles/__template.mk").read_text(encoding="utf-8"),
            encoding="utf-8", newline="\n",
        )
        (child / "makelocal.mk").write_text(
            "TEST_SRCS := $(WORKSPACE_DIR)/source/sample.c\n", encoding="utf-8"
        )
        result = self.run_tests("PathSpaces.Pass")
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertTrue((self.leaf / "results/all_tests/coverage.xml").exists(), result.stdout)
        self.assertIn("source/sample.c", result.stdout)


if __name__ == "__main__":
    unittest.main()
