"""bin_test_timeout.py の倍率の決め方を検証する。"""

import os
import sys
import unittest
from unittest.mock import patch

# 埋め込み版 Python (._pth) はスクリプトのディレクトリを sys.path に入れないため、明示的に追加する。
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import bin_test_timeout


class TimeoutScaleTest(unittest.TestCase):
    def test_default_scale_depends_on_os(self):
        with patch.dict(os.environ, {bin_test_timeout.SCALE_ENV: ""}):
            expected = bin_test_timeout.WINDOWS_SCALE if os.name == "nt" else 1.0
            self.assertEqual(bin_test_timeout.scaled_timeout(60), 60 * expected)

    def test_environment_overrides_default(self):
        with patch.dict(os.environ, {bin_test_timeout.SCALE_ENV: "2.5"}):
            self.assertEqual(bin_test_timeout.scaled_timeout(60), 150)

    def test_invalid_scale_is_rejected(self):
        for value in ("abc", "0", "-1"):
            with self.subTest(value=value), patch.dict(os.environ, {bin_test_timeout.SCALE_ENV: value}):
                with self.assertRaises(ValueError):
                    bin_test_timeout.scaled_timeout(60)


if __name__ == "__main__":
    unittest.main()
