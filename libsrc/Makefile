SUBDIRS = $(wildcard */)

.DEFAULT_GOAL := default

.PHONY: default
ifeq ($(OS),Windows_NT)
    # Windows: 4種類のランタイムライブラリをビルド
default:
	@$(MAKE) build-all-windows
else
    # Linux: 通常ビルド
default: $(SUBDIRS)
endif

.PHONY: clean
ifeq ($(OS),Windows_NT)
    # Windows: 4種類のランタイムライブラリをクリーン
clean:
	@$(MAKE) clean-all-windows
else
    # Linux: 通常クリーン
clean: $(SUBDIRS)
endif

.PHONY: test
test : $(SUBDIRS)

# Windows 用: 4種類のランタイムライブラリをビルド
# Build all 4 runtime library variants for Windows
.PHONY: build-all-windows
build-all-windows:
	$(MAKE) CONFIG=Release MSVC_CRT=shared $(SUBDIRS)
	$(MAKE) CONFIG=Debug MSVC_CRT=shared $(SUBDIRS)
	$(MAKE) CONFIG=Release MSVC_CRT=static $(SUBDIRS)
	$(MAKE) CONFIG=Debug MSVC_CRT=static $(SUBDIRS)

# Windows 用: 4種類のランタイムライブラリをクリーン
# Clean all 4 runtime library variants for Windows
.PHONY: clean-all-windows
clean-all-windows:
	$(MAKE) CONFIG=Release MSVC_CRT=shared $(SUBDIRS)
	$(MAKE) CONFIG=Debug MSVC_CRT=shared $(SUBDIRS)
	$(MAKE) CONFIG=Release MSVC_CRT=static $(SUBDIRS)
	$(MAKE) CONFIG=Debug MSVC_CRT=static $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/Makefile ]; then \
		echo $(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)); \
		$(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)) || exit 1; \
	else \
		echo "Skipping directory '$@' (no Makefile)"; \
	fi
