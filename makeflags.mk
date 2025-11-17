ifneq ($(OS),Windows_NT)
    # Linux
    CFLAGS=\
		-Wall \
		-Wextra \
		-Wcast-align \
		-Wdisabled-optimization \
		-Wfloat-equal \
		-Wformat=2 \
		-Winit-self \
		-Winline \
		-Winvalid-pch \
		-Wmissing-format-attribute \
		-Wmissing-noreturn \
		-Wpacked \
		-Wpadded \
		-Wredundant-decls \
		-Wstrict-aliasing=2 \
		-Wswitch-default \
		-Wswitch-enum \
		-Wundef \
		-Wvariadic-macros \
		-Wshadow \
		-Wmissing-declarations \
		-Wmissing-prototypes \
		-Wstrict-prototypes \
		-Wsequence-point \
		-Wpointer-arith \
		-Wdeclaration-after-statement \
		-Wbad-function-cast \
		-Wunsafe-loop-optimizations \
		-Wnested-externs \
		-Wcast-qual \
		-Wint-to-pointer-cast \
		-Wpointer-to-int-cast \
		-Wimplicit-fallthrough=0 \
		-Wformat-overflow=0 \
		-Wunknown-pragmas \
		-W
    CXXFLAGS=\
		-Wall \
		-Wextra \
		-Wcast-align \
		-Wdisabled-optimization \
		-Wfloat-equal \
		-Wformat=2 \
		-Winit-self \
		-Winline \
		-Winvalid-pch \
		-Wmissing-format-attribute \
		-Wmissing-noreturn \
		-Wpacked \
		-Wpadded \
		-Wredundant-decls \
		-Wstrict-aliasing=2 \
		-Wswitch-default \
		-Wswitch-enum \
		-Wundef \
		-Wvariadic-macros \
		-Wshadow \
		-Wmissing-declarations \
		-Wsequence-point \
		-Wpointer-arith \
		-Wunsafe-loop-optimizations \
		-Wcast-qual \
		-Wint-to-pointer-cast \
		-Wimplicit-fallthrough=0 \
		-Wformat-overflow=0 \
		-Wunknown-pragmas \
		-W
    LDFLAGS     =
else
    # Windows
    # UCRT (Universal C Runtime) のパスを環境変数 UniversalCRTSdkDir と UCRTVersion から取得
    UCRT_SDK_DIR := $(shell cmd /c echo %UniversalCRTSdkDir% 2>nul)
    UCRT_VERSION := $(shell cmd /c echo %UCRTVersion% 2>nul)
    UCRT_SDK_DIR_FIXED := $(subst \,/,$(UCRT_SDK_DIR))
    UCRT_DIR := $(UCRT_SDK_DIR_FIXED)Include/$(UCRT_VERSION)/ucrt
    $(info [makeflags.mk] Using UCRT from SDK environment variables: $(UCRT_DIR))

    CFLAGS      = -DUCRT_INCLUDE_DIR=\"$(UCRT_DIR)\"
    CXXFLAGS    = -DUCRT_INCLUDE_DIR=\"$(UCRT_DIR)\"
    LDFLAGS     =
endif

LIBSDIR     =

TARGETDIR  :=\
	$(WORKSPACE_FOLDER)/testfw/lib
