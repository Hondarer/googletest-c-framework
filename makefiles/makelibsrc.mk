SHELL := /bin/bash

# 【コンパイルオプションの観点】
# ・フォルダ外の追加ソースファイル : ADD_SRCS 外から指定
# ・その他のソースファイル
#   - 上記以外のカレントディレクトリに置かれているソースファイル
# 【Compilation Options】
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
#   - ADD_SRCS に指定されていて、カレントディレクトリに配置されているもの
# 【Source Generation】
# - C source files: SRCS_C
# - C++ source files: SRCS_CPP
# - Source files requiring symbolic links: LINK_SRCS
#   - No inject or filter files
# - Source files requiring copying: CP_SRCS
#   - Has either an inject file or a filter file
# - Directly placed source files: DIRECT_SRCS
#   - ADD_SRCS files placed in the current directory

# inject, filter 判定
# Determine inject and filter files
CP_SRCS := $(foreach src,$(ADD_SRCS), \
	$(if $(or $(wildcard $(notdir $(basename $(src))).inject$(suffix $(src))), \
		$(wildcard $(notdir $(src)).filter.sh)), \
		$(src)))

DIRECT_SRCS := $(if $(filter-out $(CP_SRCS),$(ADD_SRCS)),$(shell for f in $(filter-out $(CP_SRCS),$(ADD_SRCS)); do \
	if [ -f "./$$(basename $$f)" ] && [ ! -L "./$$(basename $$f)" ]; then \
		echo $$f; \
	fi; \
	done))

LINK_SRCS := $(filter-out $(CP_SRCS),$(ADD_SRCS))

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

# コンパイル対象のソースファイル (カレントディレクトリから自動収集 + 指定ファイル)
# Collect source files for compilation (auto-detect + specified files)
SRCS_C := $(wildcard *.c) $(filter %.c,$(CP_SRCS) $(LINK_SRCS))
SRCS_CPP := $(wildcard *.cc) $(wildcard *.cpp) $(filter %.cc,$(CP_SRCS) $(LINK_SRCS)) $(filter %.cpp,$(CP_SRCS) $(LINK_SRCS))

# c_cpp_properties.json から include ディレクトリを得る
# Get include directories from c_cpp_properties.json
INCDIR := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_include_paths.sh)

OBJDIR := obj

# .c を gcc でコンパイルする場合、
# あらかじめ CC に g++ を設定しておく。
# そうでない場合は、gcc とする。
# If .c source files are compiled, set CC to g++ by default, otherwise gcc
ifneq ($(CC),g++)
	CC := gcc
endif

CPP := g++

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

# -fPIC オプションが含まれていない場合に追加
# Add -fPIC option if not already included
ifeq ($(findstring -fPIC,$(CCOMFLAGS)),)
	CCOMFLAGS += -fPIC
endif
ifeq ($(findstring -fPIC,$(CPPCOMFLAGS)),)
	CPPCOMFLAGS += -fPIC
endif

# c_cpp_properties.json の defines にある値を -D として追加する
# DEFINES は prepare.mk で設定されている
# Add defines from c_cpp_properties.json to CCOMFLAGS
CCOMFLAGS += $(addprefix -D,$(DEFINES))
CPPCOMFLAGS += $(addprefix -D,$(DEFINES))

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJDIR)/$*.d
CFLAGS := $(CCOMFLAGS) $(addprefix -I, $(INCDIR))
CPPFLAGS := $(CPPCOMFLAGS) $(addprefix -I, $(INCDIR))

# OBJS
OBJS := $(filter-out $(OBJDIR)/%.inject.o, \
	$(sort $(addprefix $(OBJDIR)/, \
	$(notdir $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(patsubst %.cpp, %.o, $(SRCS_C) $(SRCS_CPP))))))))
# DEPS
DEPS := $(patsubst %.o, %.d, $(OBJS))

# BUILD の設定 (デフォルトは static)
# BUILD setting (default is static)
# make BUILD=shared で、shared となる
ifeq ($(BUILD),)
	BUILD := static
endif

# アーカイブのディレクトリ名とアーカイブ名
# TARGETDIR := . の場合、カレントディレクトリにアーカイブを生成する
# If TARGETDIR := ., the archive is created in the current directory
ifeq ($(TARGETDIR),)
	TARGETDIR := $(WORKSPACE_FOLDER)/test/lib
endif
# ディレクトリ名をアーカイブ名にする
# Use directory name as archive name if TARGET is not specified
ifeq ($(TARGET),)
	ifeq ($(BUILD),shared)
		TARGET := lib$(shell basename `pwd`).so
	else
		TARGET := lib$(shell basename `pwd`).a
	endif
endif

# アーカイブまたは共有ライブラリの生成
# Make the archive or shared library
ifeq ($(BUILD),shared)
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	$(CC) -shared -o $@ $(OBJS)
else
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	ar rvs $@ $(OBJS)
endif

# コンパイル時の依存関係に $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) を定義しているのは
# ヘッダ類などを引き込んでおく必要がある場合に、先に処理を行っておきたいため
# We define $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) as compile-time dependencies to ensure all headers are processed first

# C ソースファイルのコンパイル
# Compile C source files
$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) | $(OBJDIR) $(TARGETDIR)
	set -o pipefail; LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf

# C++ ソースファイルのコンパイル (*.cc)
# Compile C++ source files (*.cc)
$(OBJDIR)/%.o: %.cc $(OBJDIR)/%.d $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) | $(OBJDIR) $(TARGETDIR)
	set -o pipefail; LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf

# C++ ソースファイルのコンパイル (*.cpp)
# Compile C++ source files (*.cpp)
$(OBJDIR)/%.o: %.cpp $(OBJDIR)/%.d $(notdir $(LINK_SRCS)) $(notdir $(CP_SRCS)) | $(OBJDIR) $(TARGETDIR)
	set -o pipefail; LANG=$(FILES_LANG) $(CPP) $(DEPFLAGS) $(CPPFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf

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
		cat $(2) | sh $(1).filter.sh > $(1); \
		diff $(2) $(1); set $?=0; \
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

.PHONY: all
all: $(TARGETDIR)/$(TARGET)

.PHONY: clean
clean:
	-rm -f $(TARGETDIR)/$(TARGET)
#	@if [ -f $(TARGETDIR)/$(TARGET) ]; then \
#		for obj in $(OBJS); do \
#			echo ar d $(TARGETDIR)/$(TARGET) $$(basename $$obj); \
#			ar d $(TARGETDIR)/$(TARGET) $$(basename $$obj); \
#		done; \
#	fi
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

.PHONY: test
test: $(TARGETDIR)/$(TARGET)
