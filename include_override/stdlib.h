#ifndef _OVERRIDE_STDLIB_H_
#define _OVERRIDE_STDLIB_H_

/* 本物を include */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wpadded"
#include "/usr/include/stdlib.h"
#pragma GCC diagnostic pop
#else // _WIN32
/* UCRT_INCLUDE_DIR からの相対パスで 本物を include */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define MAKE_UCRT_PATH(header) TOSTRING(UCRT_INCLUDE_DIR / header)
#include MAKE_UCRT_PATH(stdlib.h)
#undef MAKE_UCRT_PATH
#undef TOSTRING
#undef STRINGIFY
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDLIB_H_
#include <mock_stdlib.h>
#undef _IN_OVERRIDE_HEADER_STDLIB_H_

#endif // _OVERRIDE_STDLIB_H_
