"""bin_test が起動する子プロセスの待ち時間 (タイムアウト) を決める。

待ち時間は処理の停止 (ハング) を検出するためのもので、速度の検証には使わない。
各テストの秒数は Linux での所要時間を基準にしている。

Windows の Git Bash はプロセスの起動が遅く、ウイルス対策ソフトの検査も加わる。
実測では、Linux で数秒の exec_test_c_cpp.sh の 1 回の実行が 50 秒から 97 秒かかった。
そのため Windows では既定で 4 倍にする。

環境変数 BIN_TEST_TIMEOUT_SCALE に倍率を指定すると、OS の既定より優先する。
例: BIN_TEST_TIMEOUT_SCALE=8 make test
"""

import os

SCALE_ENV = "BIN_TEST_TIMEOUT_SCALE"
WINDOWS_SCALE = 4.0


def timeout_scale():
    """待ち時間の倍率を返す。"""
    value = os.environ.get(SCALE_ENV, "").strip()
    if value:
        try:
            scale = float(value)
        except ValueError:
            raise ValueError(f"{SCALE_ENV} には正の数を指定してください: {value!r}") from None
        if scale <= 0:
            raise ValueError(f"{SCALE_ENV} には正の数を指定してください: {value!r}")
        return scale
    return WINDOWS_SCALE if os.name == "nt" else 1.0


def scaled_timeout(seconds):
    """Linux 基準の秒数に倍率を掛けた待ち時間を返す。"""
    return seconds * timeout_scale()
