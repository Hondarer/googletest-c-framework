#ifndef _OVERRIDE_STDIO_H_
#define _OVERRIDE_STDIO_H_

/* 本物を include */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include_next <stdio.h>
#pragma GCC diagnostic pop
#else // _WIN32
#pragma push_macro("_INC_STDIO")
#undef _INC_STDIO
#include <../ucrt/stdio.h>
#pragma pop_macro("_INC_STDIO")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDIO_H_
#include <mock_stdio.h>
#undef _IN_OVERRIDE_HEADER_STDIO_H_

#endif // _OVERRIDE_STDIO_H_
