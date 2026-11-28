# WINDOWS ではビルドをスキップ (libsystemd は Linux 専用 API)
SKIP_BUILD = WINDOWS

ifdef PLATFORM_LINUX
    # c_cpp_properties.json 同期評価は Windows ホストでも Linux 構成で実行されるため、
    # ホスト環境のプローブは実ビルド時 (MAKEFW_SYNC_EVAL 未定義時) のみ行う
    ifndef MAKEFW_SYNC_EVAL
        ifneq ($(shell pkg-config --exists libsystemd && echo 1),1)
            $(error libsystemd の開発ファイルが見つかりません。systemd-devel または libsystemd-dev をインストールしてください)
        endif
    endif
endif
