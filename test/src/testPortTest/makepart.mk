# test_com の公開ヘルパー (testPort.h) と、その内部の関数 (testPort_impl.h) を実体で検査するため、TEST_SRCS は指定しない。
# 自分自身を子プロセスとして起動する分岐を持つため、main() をテスト側で定義する。
LINK_TEST = 1
NO_GTEST_MAIN := 1

INCDIR += $(TESTFW_HOME)/include_internal $(TESTFW_HOME)/libsrc/test_com

ifdef PLATFORM_LINUX
    LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)
else ifdef PLATFORM_WINDOWS
    LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
