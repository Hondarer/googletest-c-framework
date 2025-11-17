#ifndef _OVERRIDE_STRING_H_
#define _OVERRIDE_STRING_H_

/* 本物を include */
#ifndef _WIN32
#include "/usr/include/string.h"
#else // _WIN32
/* UCRT_INCLUDE_DIR からの相対パスで 本物を include */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define MAKE_UCRT_PATH(header) TOSTRING(UCRT_INCLUDE_DIR / header)
#include MAKE_UCRT_PATH(string.h)
#undef MAKE_UCRT_PATH
#undef TOSTRING
#undef STRINGIFY
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STRING_H_
#include <mock_string.h>
#undef _IN_OVERRIDE_HEADER_STRING_H_

#endif // _OVERRIDE_STRING_H_
