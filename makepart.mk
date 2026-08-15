# GoogleTest と OS / libc の mock ヘッダーは外部定義の構造体をそのまま
# include するため、-Wpadded の false positive が発生する。
# 従い、framework/gtest では -Wpadded は指定しない。
GCC_WARN_BASE = \
	-Wall -Wextra \
	-Wformat=2 \
	-Wshadow -Wundef \
	-Wpointer-arith -Wcast-qual -Wcast-align \
	-Wswitch-enum -Wswitch-default \
	-Wpacked \
	-Wunknown-pragmas \
	-Wconversion \
	-Wsign-conversion
GCC_WARN_C_ONLY = \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wmissing-declarations

ifneq ($(OS),Windows_NT)
    # Linux
    CFLAGS      = $(GCC_WARN_BASE) $(GCC_WARN_C_ONLY)
    CXXFLAGS    = $(GCC_WARN_BASE)
    LDFLAGS     =
else
    # Windows
    CFLAGS      =
    CXXFLAGS    =
    LDFLAGS     =
endif

ifeq ($(strip $(TESTFW_HOME)),)
    $(error $(TESTFW_HOME_ERROR))
endif
ifeq ($(wildcard $(TESTFW_HOME)),)
    $(error $(TESTFW_HOME_ERROR))
endif

INCDIR += \
    $(TESTFW_HOME)/gtest/include \
    $(TESTFW_HOME)/include \
    $(TESTFW_HOME)/include_internal

LIBSDIR     =
