#ifndef _OVERRIDE_SYS_STAT_H_
#define _OVERRIDE_SYS_STAT_H_

/* 本物を include */
#ifndef _WIN32
#include "/usr/include/sys/stat.h"
#else // _WIN32
/* UCRT_INCLUDE_DIR からの相対パスで 本物を include */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define MAKE_UCRT_PATH(header) TOSTRING(UCRT_INCLUDE_DIR / header)
#include MAKE_UCRT_PATH(sys\stat.h)
#undef MAKE_UCRT_PATH
#undef TOSTRING
#undef STRINGIFY
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STAT_H_
#include <sys/mock_stat.h>
#undef _IN_OVERRIDE_HEADER_STAT_H_

#endif // _OVERRIDE_SYS_STAT_H_
