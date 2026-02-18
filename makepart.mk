ifneq ($(OS),Windows_NT)
    # Linux
    WARN_BASE = \
		-Wall -Wextra \
		-Wformat=2 \
		-Wshadow -Wundef \
		-Wpointer-arith -Wcast-qual -Wcast-align \
		-Wswitch-enum -Wswitch-default \
		-Wpacked -Wpadded \
		-Wunknown-pragmas
    WARN_C_ONLY = \
		-Wmissing-prototypes \
		-Wstrict-prototypes \
		-Wmissing-declarations
    CFLAGS     += $(WARN_BASE) $(WARN_C_ONLY)
    CXXFLAGS   += $(WARN_BASE)
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
