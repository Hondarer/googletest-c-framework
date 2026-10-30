# ライブラリの出力先をプラットフォームごとに設定
# Set library output directory based on platform
ifeq ($(strip $(TESTFW_HOME)),)
    $(error $(TESTFW_HOME_ERROR))
endif
ifeq ($(call _makefw_path_exists,$(TESTFW_HOME)),)
    $(error $(TESTFW_HOME_ERROR))
endif

ifneq ($(OS),Windows_NT)
    # Linux: TARGET_ARCH (e.g., linux_el8_x64, linux_el9_x64, linux_el10_x64)
    OUTPUT_DIR := $(TESTFW_HOME)/lib/$(TARGET_ARCH)
else
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows_x64/md)
    OUTPUT_DIR := $(TESTFW_HOME)/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif

# app の test/libsrc と同じく、モックの巨大な関数がコンパイラの上限を超えないようにする。
# Linux は GCC の変数追跡、Windows は COFF のセクション数。理由は makefw の _flags.mk を参照。
ifneq ($(OS),Windows_NT)
    CFLAGS   += -fno-var-tracking-assignments
    CXXFLAGS += -fno-var-tracking-assignments
else
    CFLAGS   += /bigobj
    CXXFLAGS += /bigobj
endif
