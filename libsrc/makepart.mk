# ライブラリの出力先をプラットフォームごとに設定
# Set library output directory based on platform
ifneq ($(OS),Windows_NT)
    # Linux: TARGET_ARCH (e.g., linux-el8-x64)
    OUTPUT_DIR := $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)
else
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
    OUTPUT_DIR := $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
