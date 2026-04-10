# ライブラリの出力先をプラットフォームごとに設定
# Set library output directory based on platform
ifeq ($(strip $(TESTFW_DIR)),)
    $(error $(TESTFW_DIR_ERROR))
endif
ifeq ($(wildcard $(TESTFW_DIR)),)
    $(error $(TESTFW_DIR_ERROR))
endif

ifneq ($(OS),Windows_NT)
    # Linux: TARGET_ARCH (e.g., linux-el8-x64)
    OUTPUT_DIR := $(TESTFW_DIR)/lib/$(TARGET_ARCH)
else
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
    OUTPUT_DIR := $(TESTFW_DIR)/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
