SUBDIRS = \
	libsrc

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
	@echo "Building all Windows library variants..."
	@echo "=== Building md (Release, Multi-threaded DLL) ==="
	$(MAKE) clean-libsrc
	$(MAKE) CONFIG=Release MSVC_CRT=shared $(SUBDIRS)
	@echo ""
	@echo "=== Building mdd (Debug, Multi-threaded DLL) ==="
	$(MAKE) clean-libsrc
	$(MAKE) CONFIG=Debug MSVC_CRT=shared $(SUBDIRS)
	@echo ""
	@echo "=== Building mt (Release, Multi-threaded Static) ==="
	$(MAKE) clean-libsrc
	$(MAKE) CONFIG=Release MSVC_CRT=static $(SUBDIRS)
	@echo ""
	@echo "=== Building mtd (Debug, Multi-threaded Static) ==="
	$(MAKE) clean-libsrc
	$(MAKE) CONFIG=Debug MSVC_CRT=static $(SUBDIRS)
	@echo ""
	@echo "All Windows library variants built successfully!"

# Windows 用: 4種類のランタイムライブラリをクリーン
# Clean all 4 runtime library variants for Windows
.PHONY: clean-all-windows
clean-all-windows:
	@echo "Cleaning all Windows library variants..."
	$(MAKE) CONFIG=Release MSVC_CRT=shared clean-libsrc
	$(MAKE) CONFIG=Debug MSVC_CRT=shared clean-libsrc
	$(MAKE) CONFIG=Release MSVC_CRT=static clean-libsrc
	$(MAKE) CONFIG=Debug MSVC_CRT=static clean-libsrc
	@echo "All Windows library variants cleaned!"

# libsrc を直接クリーン
# Clean libsrc directory directly
.PHONY: clean-libsrc
clean-libsrc:
	@$(MAKE) -C libsrc clean

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/Makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "Skipping directory '$@' (no Makefile)"; \
	fi
