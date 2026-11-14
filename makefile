SUBDIRS = \
	libsrc

BUILD_STAMP = $(CURDIR)/make_build.stamp

# Windows の場合、MSVC_CRT_SUBDIR が未設定なら計算する
# Calculate MSVC_CRT_SUBDIR if not set (for standalone builds)
ifeq ($(OS),Windows_NT)
    MSVC_CRT ?= shared
    CONFIG ?= RelWithDebInfo
    TESTFW_WINDOWS_VARIANTS ?= md mdd mt mtd
    ifeq ($(MSVC_CRT_SUBDIR),)
        ifeq ($(CONFIG),Debug)
            ifeq ($(MSVC_CRT),shared)
                MSVC_CRT_SUBDIR := mdd
            else
                MSVC_CRT_SUBDIR := mtd
            endif
        else
            ifeq ($(MSVC_CRT),shared)
                MSVC_CRT_SUBDIR := md
            else
                MSVC_CRT_SUBDIR := mt
            endif
        endif
    endif
endif

.DEFAULT_GOAL := default

.PHONY: default
ifeq ($(OS),Windows_NT)
default :
	@git_hash=$$(git -C "$(CURDIR)" rev-parse HEAD 2>/dev/null); \
	git_dirty=$$(git -C "$(CURDIR)" status --porcelain 2>/dev/null); \
	msvc_crt="$(MSVC_CRT_SUBDIR)"; \
	variants="$(strip $(TESTFW_WINDOWS_VARIANTS))"; \
	if [ -f "$(BUILD_STAMP)" ] && [ -n "$$msvc_crt" ]; then \
		prev_line=$$(sed -n '2p' "$(BUILD_STAMP)"); \
		prev_crt=$$(printf '%s' "$$prev_line" | sed -n 's/^MSVC_CRT=//p'); \
		if [ -n "$$prev_crt" ] && [ "$$prev_crt" != "$$msvc_crt" ]; then \
			echo "ERROR: MSVC runtime mismatch detected. Run 'make clean' first, then rebuild.  Previous build: $$prev_crt  Current request: $$msvc_crt" >&2; \
			exit 1; \
		fi; \
	fi; \
	if [ -n "$$git_hash" ] && [ -z "$$git_dirty" ] && [ -f "$(BUILD_STAMP)" ]; then \
		prev_hash=$$(sed -n '1p' "$(BUILD_STAMP)"); \
		prev_variants=$$(sed -n 's/^VARIANTS=//p' "$(BUILD_STAMP)"); \
		if [ -z "$$prev_variants" ]; then prev_variants="md mdd mt mtd"; fi; \
		if [ -n "$$msvc_crt" ]; then \
			prev_crt=$$(sed -n 's/^MSVC_CRT=//p' "$(BUILD_STAMP)"); \
			if [ "$$prev_hash" = "$$git_hash" ] && [ "$$prev_crt" = "$$msvc_crt" ] && [ "$$prev_variants" = "$$variants" ]; then \
				echo "INFO: Skipping build (already built at $$git_hash with $$msvc_crt)"; \
				exit 0; \
			fi; \
		else \
			if [ "$$prev_hash" = "$$git_hash" ] && [ "$$prev_variants" = "$$variants" ]; then \
				echo "INFO: Skipping build (already built at $$git_hash)"; \
				exit 0; \
			fi; \
		fi; \
	fi; \
	rm -f "$(BUILD_STAMP)"; \
	make_exit=0; \
	if [ -f libsrc/makefile ]; then \
		echo $(MAKE) -C libsrc; \
		$(MAKE) -C libsrc || make_exit=$$?; \
	fi; \
	if [ $$make_exit -eq 0 ] && [ -n "$$git_hash" ] && [ -z "$$git_dirty" ]; then \
		printf '%s\n' "$$git_hash" > "$(BUILD_STAMP)"; \
		if [ -n "$$msvc_crt" ]; then \
			printf 'MSVC_CRT=%s\n' "$$msvc_crt" >> "$(BUILD_STAMP)"; \
		fi; \
		printf 'VARIANTS=%s\n' "$$variants" >> "$(BUILD_STAMP)"; \
	fi; \
	if [ $$make_exit -ne 0 ]; then exit $$make_exit; fi
else
default :
	@git_hash=$$(git -C "$(CURDIR)" rev-parse HEAD 2>/dev/null); \
	git_dirty=$$(git -C "$(CURDIR)" status --porcelain 2>/dev/null); \
	if [ -n "$$git_hash" ] && [ -z "$$git_dirty" ] && \
	   [ -f "$(BUILD_STAMP)" ] && [ "$$(cat '$(BUILD_STAMP)')" = "$$git_hash" ]; then \
		echo "INFO: Skipping build (already built at $$git_hash)"; \
	else \
		rm -f "$(BUILD_STAMP)"; \
		make_exit=0; \
		if [ -f libsrc/makefile ]; then \
			echo $(MAKE) -C libsrc; \
			$(MAKE) -C libsrc || make_exit=$$?; \
		fi; \
		if [ $$make_exit -eq 0 ] && [ -n "$$git_hash" ] && [ -z "$$git_dirty" ]; then \
			echo "$$git_hash" > "$(BUILD_STAMP)"; \
		fi; \
		if [ $$make_exit -ne 0 ]; then exit $$make_exit; fi; \
	fi
endif

.PHONY: clean
clean : $(SUBDIRS)
	@$(MAKE) -C test clean
	@rm -f "$(BUILD_STAMP)"

# framework/testfw.assured.stamp のハッシュが HEAD と一致し、作業ツリーに差分が無いとき、
# test/ と bin_test/ のテストを省略する。判定は makefw の resolve_app_deps.sh --assured で行う。
# libsrc (app がリンクするライブラリ) は $(SUBDIRS) として通常どおりビルドする。
# スタンプが無いときは判定のスクリプトを起動せず、通常どおりテストする。
# bin_test/ は、埋め込み版 Python (._pth) と同じく、スクリプトのディレクトリを sys.path に入れずに実行する。
TESTFW_MAKEFW_HOME ?= $(if $(strip $(MAKEFW_HOME)),$(subst \,/,$(strip $(MAKEFW_HOME))),$(CURDIR)/../makefw)
TESTFW_ASSURED_STAMP = $(CURDIR).assured.stamp

.PHONY: test quicktest
test quicktest : $(SUBDIRS)
	@assured=0; \
	if [ -f "$(TESTFW_ASSURED_STAMP)" ]; then \
		resolver="$(TESTFW_MAKEFW_HOME)/bin_internal/resolve_app_deps.sh"; \
		if [ -f "$$resolver" ]; then \
			assured=$$(bash "$$resolver" --assured "$(CURDIR)") || exit $$?; \
		else \
			echo "Warning: $$resolver is not found. Ignoring testfw.assured.stamp." >&2; \
		fi; \
	fi; \
	if [ "$$assured" = "1" ]; then \
		echo "INFO: Skipping test and bin_test (testfw.assured.stamp matches the commit and the tree is clean)"; \
		exit 0; \
	fi; \
	$(MAKE) -C test $@ || exit $$?; \
	echo "PYTHONSAFEPATH=1 python -m unittest discover -s bin_test -p 'test_*.py'"; \
	PYTHONSAFEPATH=1 python -m unittest discover -s bin_test -p 'test_*.py'

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		if [ -n "$(MAKECMDGOALS)" ]; then \
			echo $(MAKE) -C $@ $(MAKECMDGOALS); \
			$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
		else \
			echo $(MAKE) -C $@; \
			$(MAKE) -C $@ || exit 1; \
		fi; \
	else \
		:; # echo "Skipping directory '$@' (no makefile)"; \
	fi
