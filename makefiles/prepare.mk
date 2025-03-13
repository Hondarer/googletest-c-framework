# 各 Makefile から呼び出され、
# 1. c_cpp_properties.json から defines を設定する
# 2. 親階層から Makefile の存在する階層までに存在する makeflags.mk を
#    親階層から Makefile の存在する階層に向かって順次 include する

# c_cpp_properties.json から defines を得る
DEFINES := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_defines.sh)
# defines の値を変数名 (値 = 1) として設定する
$(foreach define, $(DEFINES), $(eval $(define) = 1))

# test
#ifdef GOOGLETEST_C_SAMPLE
#$(info GOOGLETEST_C_SAMPLE: $(GOOGLETEST_C_SAMPLE));
#endif

# ソースファイルのエンコード指定から LANG を得る
FILES_LANG := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_files_lang.sh)

# test
#$(info FILES_LANG: $(FILES_LANG));

# makeflags.mk の検索
MAKEFLAGS_MK := $(shell \
	dir=`pwd`; \
	while [ "$$dir" != "/" ]; do \
		if [ -f "$$dir/makeflags.mk" ]; then \
			echo "$$dir/makeflags.mk"; \
		fi; \
		if [ -f "$$dir/.workspaceRoot" ]; then \
			break; \
		fi; \
		dir=$$(dirname $$dir); \
	done \
)

# 逆順にする
MAKEFLAGS_MK := $(foreach mkfile, $(shell seq $(words $(MAKEFLAGS_MK)) -1 1), $(word $(mkfile), $(MAKEFLAGS_MK)))

# makeflags.mk が存在すればインクルード
#$(foreach makeflags, $(MAKEFLAGS_MK), $(info include $(makeflags)) $(eval include $(makeflags)))
$(foreach makeflags, $(MAKEFLAGS_MK), $(eval include $(makeflags)))
