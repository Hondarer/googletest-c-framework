#ifndef _OVERRIDE_TIME_H
#define _OVERRIDE_TIME_H

/* 本物を include */
#ifndef _WIN32
#include_next <time.h>
#else // _WIN32
#pragma push_macro("_INC_TIME")
#undef _INC_TIME
#include <../ucrt/time.h>
#pragma pop_macro("_INC_TIME")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_TIME_H
#include <mock_time.h>
#undef _IN_OVERRIDE_HEADER_TIME_H

#endif // _OVERRIDE_TIME_H
