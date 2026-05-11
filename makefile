SUBDIRS = \
	libsrc

BUILD_LOG = $(CURDIR)/make_build.log

.DEFAULT_GOAL := default

.PHONY: default
ifeq ($(OS),Windows_NT)
default : $(SUBDIRS)
else
default :
	@git_hash=$$(git -C "$(CURDIR)" rev-parse HEAD 2>/dev/null); \
	git_dirty=$$(git -C "$(CURDIR)" status --porcelain --untracked-files=no 2>/dev/null); \
	if [ -n "$$git_hash" ] && [ -z "$$git_dirty" ] && \
	   [ -f "$(BUILD_LOG)" ] && [ "$$(cat '$(BUILD_LOG)')" = "$$git_hash" ]; then \
		echo "INFO: Skipping testfw build (already built at $$git_hash)"; \
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
