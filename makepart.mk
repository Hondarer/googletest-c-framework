GCC_WARN_BASE = \
	-Wall -Wextra \
	-Wformat=2 \
	-Wshadow -Wundef \
	-Wpointer-arith -Wcast-qual -Wcast-align \
	-Wswitch-enum -Wswitch-default \
	-Wpacked -Wpadded \
	-Wunknown-pragmas
GCC_WARN_C_ONLY = \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wmissing-declarations

ifneq ($(OS),Windows_NT)
    # Linux
    CFLAGS     += $(GCC_WARN_BASE) $(GCC_WARN_C_ONLY)
    CXXFLAGS   += $(GCC_WARN_BASE)
    LDFLAGS     =
else
    # Windows
    CFLAGS      =
    CXXFLAGS    =
    LDFLAGS     =
endif

INCDIR += \
    $(WORKSPACE_FOLDER)/testfw/gtest/include \
    $(WORKSPACE_FOLDER)/testfw/include

LIBSDIR     =
