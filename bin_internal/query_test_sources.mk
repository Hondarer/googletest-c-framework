# TEST_SRCS は makefw に解釈させ、呼び出し側へ NUL 区切りで返す。
.PHONY: __testfw_query_sources
__testfw_query_sources:
	@printf '%s\0' $(foreach src,$(TEST_SRCS),"$(src)") > "$(MAKEFW_TEST_SRC_REPORT_FILE)"
