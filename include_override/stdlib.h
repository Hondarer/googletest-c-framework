#ifndef _OVERRIDE_STDLIB_H
#define _OVERRIDE_STDLIB_H

/* 本物を include */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wpadded"
#include_next <stdlib.h>
#pragma GCC diagnostic pop
#else // _WIN32
#pragma push_macro("_INC_STDLIB")
#undef _INC_STDLIB
#include <../ucrt/stdlib.h>
#pragma pop_macro("_INC_STDLIB")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDLIB_H
#include <mock_stdlib.h>
#undef _IN_OVERRIDE_HEADER_STDLIB_H

#endif // _OVERRIDE_STDLIB_H
