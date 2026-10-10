"""空白を含む配置先から make 経由で .NET テストと TRX の集計を検証する。"""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
from test_results_markdown import assert_results

# Windows の subprocess は System32 を PATH より先に探すため、名前だけで起動すると
# WSL の bash.exe を選ぶことがある。PATH 上の bash (Git Bash など) を明示して使う。
# see: https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw
BASH = shutil.which("bash") or "bash"


TESTFW = Path(__file__).resolve().parents[1]
MAKEFW = TESTFW.parent / "makefw"


class DotnetPathSpacesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="dotnet testfw space ")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.root = Path(cls.temp.name)
        cls.leaf = cls.root / "framework/fixture/test/src/ExampleTests"
        cls.leaf.mkdir(parents=True)
        (cls.root / ".workspaceRoot").touch()
        for directory in ("makefiles", "bin_internal"):
            shutil.copytree(MAKEFW / directory, cls.root / "framework/makefw" / directory)
        shutil.copytree(TESTFW / "bin_internal", cls.root / "framework/testfw/bin_internal")
        with open(cls.leaf / "makefile", "w", encoding="utf-8", newline="\n") as handle:
            handle.write((MAKEFW / "makefiles/__template.mk").read_text(encoding="utf-8"))
        sdk = subprocess.check_output(["dotnet", "--version"], text=True, encoding="utf-8").strip().split(".")[0]
        (cls.leaf / "ExampleTests.csproj").write_text(f'''<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net{sdk}.0</TargetFramework>
    <IsTestProject>true</IsTestProject>
    <IsPackable>false</IsPackable>
  </PropertyGroup>
  <ItemGroup>
    <PackageReference Include="Microsoft.NET.Test.Sdk" Version="17.11.1" />
    <PackageReference Include="xunit" Version="2.9.2" />
    <PackageReference Include="xunit.runner.visualstudio" Version="2.8.2" />
  </ItemGroup>
</Project>
''', encoding="utf-8")
        (cls.leaf / "ExampleTests.cs").write_text('''using Xunit;
using System.Collections.Generic;
public class ExampleTests {
    [Fact]
    public void Pass() {
        // Arrange
        int value = 1;
        // Act
        int actual = value + 1;
        // Assert
        Assert.Equal(2, actual); // [確認_正常系] - 値が一致すること。
    }
    [Theory]
    [InlineData(1)] [InlineData(2)] [InlineData(3)]
    public void Branch(int value) {
        if (value < 3) {
            Assert.True(value < 3); // [確認_正常系 回数=1+1] - 3 未満であること。
        } else {
            Assert.Equal(3, value); // [確認_異常系] - 3 であること。
        }
    }
    public static IEnumerable<object[]> Rows() {
        yield return new object[] { 1 };
        yield return new object[] { 2 };
        yield return new object[] { 3 };
    }
    [Theory]
    [MemberData(nameof(Rows))]
    public void Uniform(int value) {
        for (int i = 0; i < 3; ++i) {
            Assert.True(value > 0); // [確認_正常系 回数=PARAM*(1+2)] - 正の値であること。
        }
    }
}
''', encoding="utf-8")
        (cls.leaf / "SubprocedureTests.cs").write_text('''using Xunit;
using System;
public class SubprocedureTests : IDisposable {
    // [サブ手順 名前=SubprocedureTests.Constructor]
    public SubprocedureTests() {
        Assert.True(true); // [確認_正常系] - 準備が完了すること。
    }
    // [サブ手順終了]

    // [サブ手順 名前=SubprocedureTests.Check]
    private void Check() {
        Assert.Equal(2, 1 + 1); // [確認_正常系] - 加算の結果が 2 であること。
    }
    // [サブ手順終了]

    // [サブ手順参照 名前=SubprocedureTests.Constructor]
    [Fact]
    public void Case() {
        for (int i = 0; i < 2; ++i) {
            Check(); // [サブ手順参照 名前=SubprocedureTests.Check 回数=2]
        }
    }
    // [サブ手順参照 名前=SubprocedureTests.Dispose]

    // [サブ手順 名前=SubprocedureTests.Dispose]
    public void Dispose() {
        Assert.True(true); // [確認_正常系] - 後処理が完了すること。
    }
    // [サブ手順終了]
}
''', encoding="utf-8")
        hooks = []
        for hook in ("pre-build", "post-build", "pre-test", "post-test"):
            hooks.append(f'{hook}:\n\t@printf "%s\\n" "{hook}" >> hooks.log\n')
        (cls.leaf / "makelocal.mk").write_text("\n".join(hooks), encoding="utf-8")
        tmpdir = cls.root / "tmp space"
        tmpdir.mkdir()
        tools = cls.root / "tools space"
        tools.mkdir()
        cls.wrapper = tools / "dotnet wrapper.sh"
        with open(cls.wrapper, "w", encoding="utf-8", newline="\n") as handle:
            handle.write('#!/bin/bash\nprintf "%s\\n" "$*" >> "$DOTNET_CALL_LOG"\nexec dotnet "$@"\n')
        cls.wrapper.chmod(0o755)
        cls.env = {
            key: value for key, value in os.environ.items()
            if not key.startswith("MAKEFW_")
            and key not in ("MAKEFLAGS", "MFLAGS", "WORKSPACE_DIR", "MYAPP_DIR", "APP_DIR")
        }
        cls.env.update(
            MAKEFW_HOME=(cls.root / "framework/makefw").as_posix(),
            TESTFW_HOME=(cls.root / "framework/testfw").as_posix(),
            TMPDIR=tmpdir.as_posix(), DOTNET=cls.wrapper.as_posix(),
            DOTNET_CALL_LOG=(cls.root / "dotnet calls.log").as_posix(),
        )

    def test_make_build_and_test(self):
        result = subprocess.run(
            ["make", "--no-print-directory", "test", "JOBS=1"],
            cwd=self.leaf, env=self.env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            encoding="utf-8", errors="replace", timeout=240,
        )
        self.assertEqual(result.returncode, 0, result.stdout)
        log = (self.leaf / "results/ExampleTests.Pass/results.md").read_text(encoding="utf-8")
        self.assertIn("// Arrange", log)
        self.assertIn("### 確認内容 (正常系:1)", log)
        branch = (self.leaf / "results/ExampleTests.Branch/results.md").read_text(encoding="utf-8")
        self.assertIn("### 確認内容 (正常系:2, 異常系:1)", branch)
        uniform = (self.leaf / "results/ExampleTests.Uniform/results.md").read_text(encoding="utf-8")
        self.assertIn("### 確認内容 (正常系:9)", uniform)
        self.assertIn("PARAM=3", uniform)
        subprocedure = (self.leaf / "results/SubprocedureTests.Case/results.md").read_text(encoding="utf-8")
        self.assertIn("### 確認内容 (正常系:4)", subprocedure)
        self.assertIn("\nprivate void Check()", subprocedure)
        self.assertEqual(subprocedure.count("// サブ手順: SubprocedureTests.Check\n"), 1)
        self.assertIn("SubprocedureTests.cs:", subprocedure)
        assert_results(self, self.leaf / "results")
        summary = (self.leaf / "results/all_tests/summary.md").read_text(encoding="utf-8")
        self.assertIn("[`ExampleTests.Pass`](../ExampleTests.Pass/results.md) | PASSED", summary)
        hooks = (self.leaf / "hooks.log").read_text(encoding="utf-8").splitlines()
        self.assertEqual(hooks, ["pre-build", "post-build", "pre-test", "post-test"])
        calls = (self.root / "dotnet calls.log").read_text(encoding="utf-8")
        self.assertIn("build ", calls)
        self.assertIn("test --list-tests", calls)
        self.assertIn("--results-directory", calls)
        for warning in self.leaf.rglob("*.warn"):
            self.assertEqual(warning.stat().st_size, 0, warning.read_text(encoding="utf-8", errors="replace"))

        source = self.leaf / "ExampleTests.cs"
        original = source.read_text(encoding="utf-8")
        try:
            source.write_text(original.replace("[確認_正常系]", "[確認_正常系 回数=1+]", 1), encoding="utf-8")
            failed = subprocess.run(
                [BASH, str(self.root / "framework/testfw/bin_internal/exec_test_dotnet.sh")],
                cwd=self.leaf, env=dict(self.env, CONFIG="RelWithDebInfo"),
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                encoding="utf-8", errors="replace", timeout=60,
            )
            self.assertEqual(failed.returncode, 1, failed.stdout)
            failed_log = (self.leaf / "results/ExampleTests.Pass/results.md").read_text(encoding="utf-8")
            self.assertIn("ExampleTests.Pass:抽出コード:", failed_log)
        finally:
            source.write_text(original, encoding="utf-8")

    def test_missing_sdk_is_reported(self):
        env = dict(self.env, DOTNET=(self.root / "missing sdk/dotnet").as_posix(), CONFIG="Debug")
        result = subprocess.run(
            [BASH, str(TESTFW / "bin_internal/exec_test_dotnet.sh")],
            cwd=self.leaf, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            encoding="utf-8", errors="replace", timeout=30,
        )
        self.assertNotEqual(result.returncode, 0, result.stdout)
        self.assertIn("--list-tests failed", result.stdout)


if __name__ == "__main__":
    unittest.main()
