SHELL := /bin/bash

# 【コンパイルオプションの観点】
# ・テストの対象 (カバレッジ対象) のソースファイル : TEST_SRCS 外から指定
# ・フォルダ外の追加ソースファイル : ADD_SRCS 外から指定
# ・その他のソースファイル
#   - 上記以外のカレントディレクトリに置かれているソースファイル
# 【Compilation Options】
# - Source files subject to testing (coverage targets): TEST_SRCS (specified externally)
# - Additional source files outside the folder: ADD_SRCS (specified externally)
# - Other source files
#   - Source files located in the current directory except for the above

# 【ソース生成の観点】
# ・C のソース : SRCS_C
# ・C++ のソース : SRCS_CPP
# ・シンボリックリンクが必要なソースファイル : LINK_SRCS
#   - inject ファイル および フィルタファイルがない
# ・コピーが必要なソースファイル : CP_SRCS
#   - inject ファイル または フィルタファイルがある
# ・直接配置のソースファイル : DIRECT_SRCS
#   - TEST_SRCS, ADD_SRCS に指定されていて、カレントディレクトリに配置されているもの
# 【Source Generation】
# - C source files: SRCS_C
# - C++ source files: SRCS_CPP
# - Source files requiring symbolic links: LINK_SRCS
#   - No inject or filter files
# - Source files requiring copying: CP_SRCS
#   - Has either an inject file or a filter file
# - Directly placed source files: DIRECT_SRCS
#   - TEST_SRCS and ADD_SRCS files placed in the current directory

# inject, filter 判定
# Determine inject and filter files
CP_SRCS := $(foreach src,$(TEST_SRCS) $(ADD_SRCS), \
	$(if $(or $(wildcard $(notdir $(basename $(src))).inject$(suffix $(src))), \
		$(wildcard $(notdir $(src)).filter.sh)), \
		$(src)))

DIRECT_SRCS := $(if $(filter-out $(CP_SRCS),$(TEST_SRCS) $(ADD_SRCS)),$(shell for f in $(filter-out $(CP_SRCS),$(TEST_SRCS) $(ADD_SRCS)); do \
	if [ -f "./$$(basename $$f)" ] && [ ! -L "./$$(basename $$f)" ]; then \
		echo $$f; \
	fi; \
	done))

LINK_SRCS := $(filter-out $(CP_SRCS) $(DIRECT_SRCS),$(TEST_SRCS) $(ADD_SRCS))

# 以下の処理は、ADD_SRCS に inject ファイルや filter ファイルを指定するための追加処理
# make 開始時点でファイルが配置されていない場合は、CP_SRCS に正しく移動しきれないファイルがあるため
# This additional process allows specifying inject/filter files under ADD_SRCS before make begins, in case files aren't placed initially

# LINK_SRCS の中から `*.inject.*` に対応する元ファイルを探して CP_SRCS に追加
# Add original files matching `*.inject.*` in LINK_SRCS to CP_SRCS
CP_SRCS += $(foreach f, $(LINK_SRCS), \
	$(if $(findstring .inject.,$(notdir $(f))), \
		$(foreach src, $(filter %$(subst .inject.,.,$(notdir $(f))), $(LINK_SRCS)), $(src))))

# LINK_SRCS の中から `*.filter.sh` に対応する元ファイルを探して CP_SRCS に追加
# Add original files matching `*.filter.sh` in LINK_SRCS to CP_SRCS
CP_SRCS += $(foreach f, $(LINK_SRCS), \
	$(if $(findstring .filter.sh,$(notdir $(f))), \
		$(foreach src, $(filter %$(subst .filter.sh,,$(notdir $(f))), $(LINK_SRCS)), $(src))))

# CP_SRCS の重複排除
# Remove duplicate entries from CP_SRCS
CP_SRCS := $(sort $(CP_SRCS))

# LINK_SRCS から CP_SRCS のファイルを削除
# Remove CP_SRCS files from LINK_SRCS
LINK_SRCS := $(filter-out $(CP_SRCS), $(LINK_SRCS))

#$(info CP_SRCS: $(CP_SRCS))
#$(info DIRECT_SRCS: $(DIRECT_SRCS))
#$(info LINK_SRCS: $(LINK_SRCS))

# gcovr のフィルタを作成
# gcovr では、シンボリックリンクの場合は、実パスを与える必要がある
# Create filters for gcovr (symbolic links require real paths)
GCOVR_SRCS := $(foreach src,$(TEST_SRCS), \
	$(if $(filter $(src),$(LINK_SRCS)), \
		 $(src), \
		 $(notdir $(src))))

# コンパイル対象のソースファイル (カレントディレクトリから自動収集 + 指定ファイル)
# Collect source files for compilation (auto-detect + specified files)
SRCS_C := $(wildcard *.c) $(filter %.c,$(CP_SRCS) $(LINK_SRCS))
SRCS_CPP := $(wildcard *.cc) $(wildcard *.cpp) $(filter %.cc,$(CP_SRCS) $(LINK_SRCS)) $(filter %.cpp,$(CP_SRCS) $(LINK_SRCS))

# c_cpp_properties.json から include ディレクトリを得る
# Get include directories from c_cpp_properties.json
INCDIR := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_include_paths.sh)

# 外部で LIBSDIR が指定されている場合は維持して結合
# Merge external LIBSDIR if specified
# LINK_TEST が 1 の場合にのみ設定する
ifeq ($(LINK_TEST), 1)
LIBSDIR := $(LIBSDIR) \
	$(WORKSPACE_FOLDER)/testfw/lib \
	$(WORKSPACE_FOLDER)/test/lib
endif

LIBSFILES := $(shell for dir in $(LIBSDIR); do [ -d $$dir ] && find $$dir -maxdepth 1 -type f; done)

# テストライブラリの設定
# Set test libraries
# LINK_TEST が 1 の場合にのみ設定する
ifeq ($(LINK_TEST), 1)
	TEST_LIBS := -lgtest_main -lgtest -lpthread -lgmock -lgcov
	ifneq ($(NO_GTEST_MAIN),)
		ifeq ($(NO_GTEST_MAIN), 1)
			TEST_LIBS := $(filter-out -lgtest_main, $(TEST_LIBS))
		endif
	endif
endif

TESTSH := $(WORKSPACE_FOLDER)/testfw/cmnd/exec_test.sh

OBJDIR := obj
GCOVDIR := gcov
LCOVDIR := lcov

# .c を gcc でコンパイルする場合、
# あらかじめ CC に g++ を設定しておく。
# そうでない場合は、gcc とする。
# If .c source files are compiled, set CC to g++ by default, otherwise gcc
ifneq ($(CC),g++)
	CC := gcc
endif

CPP := g++

# LD が定義されていたら、定義値を優先し、定義がなければ LD は g++ とする。
ifneq ($(LD),)
	LD := g++
endif

# gtest が C++ versions less than C++17 are not supported. のため、
# -std=c++17 とする
ifeq ($(strip $(CPP_STANDARD)),)
	CPP_STANDARD := c++17
endif
CPPCOMFLAGS += -std=$(CPP_STANDARD)

# -g オプションが含まれていない場合に追加
# Add -g option if not already included
ifeq ($(findstring -g,$(CCOMFLAGS)),)
	CCOMFLAGS += -g
endif
ifeq ($(findstring -g,$(CPPCOMFLAGS)),)
	CPPCOMFLAGS += -g
endif

# c_cpp_properties.json の defines にある値を -D として追加する
# DEFINES は prepare.mk で設定されている
# Add defines from c_cpp_properties.json to CCOMFLAGS
CCOMFLAGS += $(addprefix -D,$(DEFINES))
CPPCOMFLAGS += $(addprefix -D,$(DEFINES))

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJDIR)/$*.d

# NOTE: テスト対象の場合は、CCOMFLAGS の後、通常の include の前に include_override を追加する
#       CCOMFLAGS に追加した include パスは、include_override より前に評価されるので
#       個別のテストでの include 注入に対応できる
# NOTE: For test targets, add include_override after CCOMFLAGS but before normal includes, so that test-specific includes can override

# テスト対象
# For test targets
CFLAGS_TEST := $(CCOMFLAGS) -I$(WORKSPACE_FOLDER)/testfw/include_override -I$(WORKSPACE_FOLDER)/test/include_override $(addprefix -I, $(INCDIR))
CPPFLAGS_TEST := $(CPPCOMFLAGS) -I$(WORKSPACE_FOLDER)/testfw/include_override -I$(WORKSPACE_FOLDER)/test/include_override $(addprefix -I, $(INCDIR))
# テスト対象以外
# For non-test targets
CFLAGS := $(CCOMFLAGS) $(addprefix -I, $(INCDIR))
CPPFLAGS := $(CPPCOMFLAGS) $(addprefix -I, $(INCDIR))

LDFLAGS := $(LDCOMFLAGS) $(addprefix -L, $(LIBSDIR))

# OBJS
OBJS := $(filter-out $(OBJDIR)/%.inject.o, \
	$(sort $(addprefix $(OBJDIR)/, \
	$(notdir $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(patsubst %.cpp, %.o, $(SRCS_C) $(SRCS_CPP))))))))
# DEPS
DEPS := $(patsubst %.o, %.d, $(OBJS))

# テストプログラムのディレクトリ名と実行体名
# TARGETDIR := . の場合、カレントディレクトリに実行体を生成する
# If TARGETDIR := ., the executable is created in the current directory
ifeq ($(TARGETDIR),)
	TARGETDIR := .
endif
# ディレクトリ名を実行体名にする
# Use directory name as executable name if TARGET is not specified
ifeq ($(TARGET),)
	TARGET := $(shell basename `pwd`)
endif

ifndef NO_LINK
# 実行体の生成
# Build the executable
$(TARGETDIR)/$(TARGET): $(OBJS) $(LIBSFILES) | $(TARGETDIR)
	set -o pipefail; LANG=$(FILES_LANG) $(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(TEST_LIBS) -fdiagnostics-color=always 2>&1 | nkf
else
# リンクのみ
# Link only
$(OBJS): $(LIBSFILES)
endif

# コンパイル時の依存関係に $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) を定義しているのは
# ヘッダ類などを引き込んでおく必要がある場合に、先に処理を行っておきたいため
# We define $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) as compile-time dependencies to ensure all headers are processed first

# C ソースファイルのコンパイル
# Compile C source files
$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) | $(OBJDIR)
	@set -o pipefail; if echo $(TEST_SRCS) | grep -q $(notdir $<); then \
		echo LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
		LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
	else \
		echo LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
		LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
	fi

# C++ ソースファイルのコンパイル (*.cc)
# Compile C++ source files (*.cc)
$(OBJDIR)/%.o: %.cc $(OBJDIR)/%.d $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) | $(OBJDIR)
	@set -o pipefail; if echo $(TEST_SRCS) | grep -q $(notdir $<); then \
		echo LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
		LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
	else \
		echo LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
		LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
	fi

# C++ ソースファイルのコンパイル (*.cpp)
# Compile C++ source files (*.cpp)
$(OBJDIR)/%.o: %.cpp $(OBJDIR)/%.d $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) | $(OBJDIR)
	@set -o pipefail; if echo $(TEST_SRCS) | grep -q $(notdir $<); then \
		echo LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
		LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
	else \
		echo LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
		LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
	fi

# シンボリックリンク対象のソースファイルをシンボリックリンク
# Create symbolic links for LINK_SRCS
define generate_link_src_rule
$(1):
	ln -s $(2) $(1)
#	.gitignore に対象ファイルを追加
#	Add the file to .gitignore
	echo $(1) >> .gitignore
	@tempfile=$$(mktemp) && \
	sort .gitignore | uniq > $$tempfile && \
	mv $$tempfile .gitignore
endef

# ファイルごとの依存関係を動的に定義
# ただし、from, to が同じになる場合 (一般的には Makefile の定義ミス) はスキップ
# Dynamically define file-by-file dependencies
$(foreach link_src,$(LINK_SRCS), \
  $(if \
    $(filter-out $(notdir $(link_src)),$(link_src)), \
    $(eval $(call generate_link_src_rule,$(notdir $(link_src)),$(link_src))) \
  ) \
)

# コピー対象のソースファイルをコピーして
# 1. フィルター処理をする
# 2. inject 処理をする
# Copy target source files, then apply filter processing and inject
define generate_cp_src_rule
$(1): $(2) $(wildcard $(1).filter.sh) $(wildcard $(basename $(1)).inject$(suffix $(1))) $(filter $(1).filter.sh,$(notdir $(LINK_SRCS))) $(filter $(basename $(1)).inject$(suffix $(1)),$(notdir $(LINK_SRCS)))
	@if [ -f "$(1).filter.sh" ]; then \
		echo "cat $(2) | sh $(1).filter.sh > $(1)"; \
		cat $(2) | sh -e $(1).filter.sh > $(1) && \
		diff $(2) $(1) | nkf && set $?=0; \
	else \
		echo "cp -p $(2) $(1)"; \
		cp -p $(2) $(1); \
	fi
	@if [ -f "$(basename $(1)).inject$(suffix $(1))" ]; then \
		if [ "$$(tail -c 1 $(1) | od -An -tx1)" != " 0a" ]; then \
			echo "echo \"\" >> $(1)"; \
			echo "" >> $(1); \
		fi; \
		echo "echo \"\" >> $(1)"; \
		echo "" >> $(1); \
		echo "echo \"/* Inject from test framework */\" >> $(1)"; \
		echo "/* Inject from test framework */" >> $(1); \
		echo "echo \"#ifdef _IN_TEST_SRC_\" >> $(1)"; \
		echo "#ifdef _IN_TEST_SRC_" >> $(1); \
		echo "echo \"#include \"$(basename $(1)).inject$(suffix $(1))\"\" >> $(1)"; \
		echo "#include \"$(basename $(1)).inject$(suffix $(1))\"" >> $(1); \
		echo "echo \"#endif // _IN_TEST_SRC_\" >> $(1)"; \
		echo "#endif // _IN_TEST_SRC_" >> $(1); \
	fi
#	.gitignore に対象ファイルを追加
#	Add the file to .gitignore
	echo $(1) >> .gitignore
	@tempfile=$$(mktemp) && \
	sort .gitignore | uniq > $$tempfile && \
	mv $$tempfile .gitignore
endef

# ファイルごとの依存関係を動的に定義
# Dynamically define file-by-file dependencies
$(foreach cp_src,$(CP_SRCS),$(eval $(call generate_cp_src_rule,$(notdir $(cp_src)),$(cp_src))))

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(GCOVDIR):
	mkdir -p $@

$(LCOVDIR):
	mkdir -p $@

.PHONY: all
ifndef NO_LINK
# 実行体の生成
# Build the executable
all: $(TARGETDIR)/$(TARGET)
else
# コンパイルのみ
# Compile only
all: $(OBJS) $(LIBSFILES)
endif

.PHONY: clean
clean: clean-cov clean-test
#   シンボリックリンクされたソース、コピー対象のソースを削除する
#   Remove symbolic-linked or copied source files
	-@if [ -n "$(wildcard $(notdir $(CP_SRCS) $(LINK_SRCS)))" ]; then \
		echo rm -f $(notdir $(CP_SRCS) $(LINK_SRCS)); \
		rm -f $(notdir $(CP_SRCS) $(LINK_SRCS)); \
	fi
#	.gitignore の再生成 (コミット差分が出ないように)
#	Regenerate .gitignore (avoid commit diffs)
	-rm -f .gitignore
	@for ignorefile in $(notdir $(CP_SRCS) $(LINK_SRCS)); \
		do echo $$ignorefile >> .gitignore; \
		tempfile=$$(mktemp) && \
		sort .gitignore | uniq > $$tempfile && \
		mv $$tempfile .gitignore; \
	done
	-rm -rf $(OBJDIR)
	-rm -f $(TARGETDIR)/$(TARGET) core

.PHONY: clean-cov
clean-cov:
#	カバレッジ情報と、gcov, lcov で生成したファイルを削除する
#	Delete coverage info and files generated by gcov/lcov
	-rm -rf $(OBJDIR)/*.gcda
	-rm -rf $(OBJDIR)/*.info
	-rm -rf $(GCOVDIR)
	-rm -rf $(LCOVDIR)

.PHONY: clean-test
clean-test:
#	テスト結果フォルダを削除する
#	Delete test results folder if it exists
	-rm -rf results

# Check if both variables are empty
ifneq ($(strip $(TEST_SRCS)),)

.PHONY: take-cov
take-cov: take-gcov take-lcov take-gcovr

.PHONY: take-gcovr
take-gcovr:
# gcovr (dnf install python3.11 python3.11-pip; pip3.11 install gcovr)
# If gcovr is available, run coverage. Otherwise skip.
	@if command -v gcovr > /dev/null 2>&1; then \
		#gcovr --exclude-unreachable-branches --cobertura-pretty --output coverage.xml --filter "$(shell echo $(GCOVR_SRCS) | tr ' ' '|')" > /dev/null 2>&1; \
		gcovr --exclude-unreachable-branches --filter "$(shell echo $(GCOVR_SRCS) | tr ' ' '|')"; \
	fi

.PHONY: take-gcov
take-gcov: $(GCOVDIR)
#	gcov で生成したファイルを削除する
#	Delete any existing .gcov files
	-rm -rf $(GCOVDIR)/*
#	gcov でカバレッジ情報を取得する
#	Run gcov to collect coverage
#	-bc オプションは可読性に問題があるので、使用しない (lcov の結果で確認可能)
#	Not using -bc for readability, rely on lcov results
#	gcov -bc $(TEST_SRCS) -o $(OBJDIR)
	gcov $(TEST_SRCS) -o $(OBJDIR)
#	カバレッジ未通過の *.gcov ファイルは削除する
#	Delete *.gcov files without coverage
	@if [ -n "$$(ls *.gcov 2>/dev/null)" ]; then \
		for file in *.gcov; do \
			if ! grep -qE '^\s*[0-9]+\*?:' "$$file"; then \
				echo "rm $$file # No coverage data"; \
				rm "$$file"; \
			fi; \
		done \
	fi
	mv *.gcov $(GCOVDIR)/.

.PHONY: take-lcov
take-lcov: $(LCOVDIR)
#	lcov で生成したファイルを削除する
#	Delete any existing info files generated by lcov
	-rm -rf $(OBJDIR)/*.info
	-rm -rf $(LCOVDIR)/*
#	lcov でカバレッジ情報を取得する
#	Run lcov to collect coverage
	@if [ -s "$(shell command -v lcov 2> /dev/null)" ]; then \
		echo lcov -d $(OBJDIR) -c -o $(OBJDIR)/$(TARGET).info; \
		lcov -d $(OBJDIR) -c -o $(OBJDIR)/$(TARGET).info; \
	else \
		echo "lcov not found. Skipping."; \
	fi
#	genhtml は空のファイルを指定するとエラーを出力して終了するため
#	lcov の出力ファイルが空でないか確認してから genhtml を実行する
#	genhtml fails on empty files; verify that .info is not empty first
	@if [ -s $(OBJDIR)/$(TARGET).info ]; then \
		echo genhtml --function-coverage -o $(LCOVDIR) $(OBJDIR)/$(TARGET).info; \
		genhtml --function-coverage -o $(LCOVDIR) $(OBJDIR)/$(TARGET).info; \
	else \
		echo "No valid records found in tracefile $(OBJDIR)/$(TARGET).info."; \
	fi

else

.PHONY: take-cov
take-cov:
#	カバレッジ対象がない場合のメッセージ
#	Message for no coverage targets
	@echo "No target source files for coverage measurement."

.PHONY: take-gcovr
take-gcovr:
	@echo "No target source files for coverage measurement."

.PHONY: take-gcov
take-gcov:
	@echo "No target source files for coverage measurement."

.PHONY: take-lcov
take-lcov:
	@echo "No target source files for coverage measurement."

endif

.PHONY: test
ifndef NO_LINK
# テストの実行
# Run tests
test: $(TESTSH) $(TARGETDIR)/$(TARGET)
	@status=0; \
	export TEST_SRCS="$(TEST_SRCS)" && $(SHELL) $(TESTSH) > >(nkf) 2> >(nkf >&2) || status=$$?; \
	$(MAKE) clean-cov; \
	exit $$status
else
# 何もしない
# Do nothing
test: ;
endif
