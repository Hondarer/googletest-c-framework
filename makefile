SUBDIRS = \
	libsrc

BUILD_LOG = $(CURDIR)/make_build.log

# Windows の場合、MSVC_CRT_SUBDIR が未設定なら計算する
# Calculate MSVC_CRT_SUBDIR if not set (for standalone builds)
ifeq ($(OS),Windows_NT)
    MSVC_CRT ?= shared
    CONFIG ?= RelWithDebInfo
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
	if [ -f "$(BUILD_LOG)" ] && [ -n "$$msvc_crt" ]; then \
		prev_line=$$(sed -n '2p' "$(BUILD_LOG)"); \
		prev_crt=$$(printf '%s' "$$prev_line" | sed -n 's/^MSVC_CRT=//p'); \
		if [ -n "$$prev_crt" ] && [ "$$prev_crt" != "$$msvc_crt" ]; then \
			echo "ERROR: MSVC runtime mismatch detected. Run 'make clean' first, then rebuild.  Previous build: $$prev_crt  Current request: $$msvc_crt" >&2; \
			exit 1; \
		fi; \
	fi; \
	if [ -n "$$git_hash" ] && [ -z "$$git_dirty" ] && [ -f "$(BUILD_LOG)" ]; then \
		prev_hash=$$(sed -n '1p' "$(BUILD_LOG)"); \
		if [ -n "$$msvc_crt" ]; then \
			prev_crt=$$(sed -n 's/^MSVC_CRT=//p' "$(BUILD_LOG)"); \
			if [ "$$prev_hash" = "$$git_hash" ] && [ "$$prev_crt" = "$$msvc_crt" ]; then \
				echo "INFO: Skipping build (already built at $$git_hash with $$msvc_crt)"; \
				exit 0; \
			fi; \
		else \
			if [ "$$prev_hash" = "$$git_hash" ]; then \
				echo "INFO: Skipping build (already built at $$git_hash)"; \
				exit 0; \
			fi; \
		fi; \
	fi; \
	rm -f "$(BUILD_LOG)"; \
	make_exit=0; \
	if [ -f libsrc/makefile ]; then \
		echo $(MAKE) -C libsrc; \
		$(MAKE) -C libsrc || make_exit=$$?; \
	fi; \
	if [ $$make_exit -eq 0 ] && [ -n "$$git_hash" ] && [ -z "$$git_dirty" ]; then \
		printf '%s\n' "$$git_hash" > "$(BUILD_LOG)"; \
		if [ -n "$$msvc_crt" ]; then \
			printf 'MSVC_CRT=%s\n' "$$msvc_crt" >> "$(BUILD_LOG)"; \
		fi; \
	fi; \
	if [ $$make_exit -ne 0 ]; then exit $$make_exit; fi
else
default :
	@git_hash=$$(git -C "$(CURDIR)" rev-parse HEAD 2>/dev/null); \
	git_dirty=$$(git -C "$(CURDIR)" status --porcelain 2>/dev/null); \
	if [ -n "$$git_hash" ] && [ -z "$$git_dirty" ] && \
	   [ -f "$(BUILD_LOG)" ] && [ "$$(cat '$(BUILD_LOG)')" = "$$git_hash" ]; then \
		echo "INFO: Skipping build (already built at $$git_hash)"; \
	else \
		rm -f "$(BUILD_LOG)"; \
		make_exit=0; \
		if [ -f libsrc/makefile ]; then \
			echo $(MAKE) -C libsrc; \
			$(MAKE) -C libsrc || make_exit=$$?; \
		fi; \
		if [ $$make_exit -eq 0 ] && [ -n "$$git_hash" ] && [ -z "$$git_dirty" ]; then \
			echo "$$git_hash" > "$(BUILD_LOG)"; \
		fi; \
		if [ $$make_exit -ne 0 ]; then exit $$make_exit; fi; \
	fi
endif

.PHONY: clean
clean : $(SUBDIRS)
	@rm -f "$(BUILD_LOG)"

.PHONY: test
test : $(SUBDIRS)

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
