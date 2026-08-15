# framework 配下のテストには app/makepart.mk が適用されないため、Google Test のリンクを明示する。
LINK_TEST = 1

ifdef PLATFORM_LINUX
    LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)
else ifdef PLATFORM_WINDOWS
    LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
