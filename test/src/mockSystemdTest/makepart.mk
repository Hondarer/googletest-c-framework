# WINDOWS ではビルドをスキップ (libsystemd は Linux 専用 API)
SKIP_BUILD = WINDOWS

# framework 配下のテストには app/makepart.mk が適用されないため、Google Test のリンクを明示する。
LINK_TEST = 1

ifdef PLATFORM_LINUX
    LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)
    LIBS += mock_systemd systemd
endif

TEST_SRCS += mock_systemd_target.c
