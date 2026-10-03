"""空白を含む配置先から make 経由で .NET テストと TRX の集計を検証する。"""

import os
from pathlib import Path
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
public class ExampleTests {
    [Fact]
    public void Pass() {
        // Arrange
        int value = 1;
        // Act
        int actual = value + 1;
        // Assert
        Assert.Equal(2, actual);
    }
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
        log = (self.leaf / "results/ExampleTests.Pass/results.log").read_text(encoding="utf-8")
        self.assertIn("// Arrange", log)
        summary = (self.leaf / "results/all_tests/summary.log").read_text(encoding="utf-8")
        self.assertIn("ExampleTests.Pass\tPASSED", summary)
        hooks = (self.leaf / "hooks.log").read_text(encoding="utf-8").splitlines()
        self.assertEqual(hooks, ["pre-build", "post-build", "pre-test", "post-test"])
        calls = (self.root / "dotnet calls.log").read_text(encoding="utf-8")
        self.assertIn("build ", calls)
        self.assertIn("test --list-tests", calls)
        self.assertIn("--results-directory", calls)
        for warning in self.leaf.rglob("*.warn"):
            self.assertEqual(warning.stat().st_size, 0, warning.read_text(errors="replace"))

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
