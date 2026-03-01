# WINDOWS ではビルドをスキップ (syslog は Linux 専用 API)
SKIP_BUILD = WINDOWS

# 動的ライブラリ (shared) としてビルド (LD_PRELOAD で使用するため)
LIB_TYPE = shared
