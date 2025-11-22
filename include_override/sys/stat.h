#ifndef _OVERRIDE_SYS_STAT_H
#define _OVERRIDE_SYS_STAT_H

/* 本物を include */
#ifndef _WIN32
#include_next <sys/stat.h>
#else // _WIN32
#pragma push_macro("_INC_SYS_STAT")
#undef _INC_SYS_STAT
#include <../ucrt/sys/stat.h>
#pragma pop_macro("_INC_SYS_STAT")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STAT_H
#include <sys/mock_stat.h>
#undef _IN_OVERRIDE_HEADER_STAT_H

#endif // _OVERRIDE_SYS_STAT_H
