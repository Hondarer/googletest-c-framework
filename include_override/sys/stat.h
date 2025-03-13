#ifndef _SYS_STAT_H
/* 本物の include で define されるため、ここでは define しない */
/* #define _SYS_STAT_H */

/* 本物を include */
#include "/usr/include/sys/stat.h"

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STAT_H_
#include <sys/mock_stat.h>
#undef _IN_OVERRIDE_HEADER_STAT_H_

#endif // _SYS_STAT_H