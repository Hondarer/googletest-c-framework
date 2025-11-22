#ifndef _OVERRIDE_STRING_H
#define _OVERRIDE_STRING_H

/* 本物を include */
#ifndef _WIN32
#include_next <string.h>
#else // _WIN32
/* UCRT_INCLUDE_DIR からの相対パスで 本物を include */
#pragma push_macro("_INC_STRING")
#undef _INC_STRING
#include <../ucrt/string.h>
#pragma pop_macro("_INC_STRING")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STRING_H
#include <mock_string.h>
#undef _IN_OVERRIDE_HEADER_STRING_H

#endif // _OVERRIDE_STRING_H
