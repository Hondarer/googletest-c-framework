SUBDIRS = $(wildcard */)

MAKEFW_HOME ?= $(abspath $(CURDIR)/../../makefw)
ROOT_RUNNER = $(MAKEFW_HOME)/bin/run_ordered_subdir_target.sh
include $(MAKEFW_HOME)/makefiles/_parallel.mk
MAKEFW_ALLOW_JOB_FALLBACK := 1

.DEFAULT_GOAL := default

.PHONY: default
ifeq ($(OS),Windows_NT)
    # Windows: 4種類のランタイムライブラリをビルド
    # Windows: build all 4 runtime library variants.
default:
	@$(MAKE) build-all-windows
else
    # Linux: 通常ビルド
    # Linux: normal build.
default:
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" default $(SUBDIRS)
endif

.PHONY: clean
ifeq ($(OS),Windows_NT)
    # Windows: 4種類のランタイムライブラリをクリーン
    # Windows: clean all 4 runtime library variants.
clean:
	@$(MAKE) clean-all-windows
else
    # Linux: 通常クリーン
    # Linux: normal clean.
clean:
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" clean $(SUBDIRS)
endif

.PHONY: test
test:
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" test $(SUBDIRS)

# Windows 用: 4種類のランタイムライブラリをビルド
# Build all 4 runtime library variants for Windows.
.PHONY: build-all-windows
build-all-windows:
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=RelWithDebInfo MSVC_CRT=shared MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" default $(SUBDIRS)
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=Debug MSVC_CRT=shared MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" default $(SUBDIRS)
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=RelWithDebInfo MSVC_CRT=static MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" default $(SUBDIRS)
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=Debug MSVC_CRT=static MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" default $(SUBDIRS)

# Windows 用: 4種類のランタイムライブラリをクリーン
# Clean all 4 runtime library variants for Windows.
.PHONY: clean-all-windows
clean-all-windows:
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=RelWithDebInfo MSVC_CRT=shared MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" clean $(SUBDIRS)
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=Debug MSVC_CRT=shared MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" clean $(SUBDIRS)
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=RelWithDebInfo MSVC_CRT=static MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" clean $(SUBDIRS)
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	libsrc_jobs="$$jobs"; \
	if [ -z "$$libsrc_jobs" ]; then libsrc_jobs=1; fi; \
	CONFIG=Debug MSVC_CRT=static MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$libsrc_jobs" clean $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		if [ -n "$(filter-out $(SUBDIRS),$(MAKECMDGOALS))" ]; then \
			echo $(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)); \
			$(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)) || exit 1; \
		else \
			echo $(MAKE) -C $@; \
			$(MAKE) -C $@ || exit 1; \
		fi; \
	else \
		:; # echo "Skipping directory '$@' (no makefile)"; \
	fi
