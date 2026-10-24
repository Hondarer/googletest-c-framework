#!/usr/bin/env python3
"""テストの省略判定の対象とするスクリプトの範囲を検証する。"""

from pathlib import Path
import shutil
import subprocess
import unittest

# Windows の subprocess は System32 を PATH より先に探すため、名前だけで起動すると
# WSL の bash.exe を選ぶことがある。PATH 上の bash (Git Bash など) を明示して使う。
# see: https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw
BASH = shutil.which("bash") or "bash"

BIN_INTERNAL = Path(__file__).resolve().parents[1] / "bin_internal"


class SignatureScriptsTest(unittest.TestCase):
    def test_lists_all_testfw_scripts(self):
        result = subprocess.run([BASH, str(BIN_INTERNAL / "list_signature_scripts.sh")],
                                capture_output=True, encoding="utf-8", check=True)
        names = [Path(line).name for line in result.stdout.splitlines()]
        expected = sorted(path.name for path in BIN_INTERNAL.iterdir()
                          if path.is_file() and path.suffix in (".py", ".sh"))
        # makefw のスクリプトは makefw の list_signature_inputs.sh が定義するため含めない。
        self.assertEqual(names, expected)
        self.assertIn("list_signature_scripts.sh", names)


if __name__ == "__main__":
    unittest.main()
