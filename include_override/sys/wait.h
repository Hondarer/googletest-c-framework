#ifndef _SYS_WAIT_H
/* 本物の include で define されるため、ここでは define しない */
/* #define _SYS_WAIT_H */

/* 本物を include */
#include "/usr/include/sys/wait.h"

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_WAIT_H_
#include <sys/mock_wait.h>
#undef _IN_OVERRIDE_HEADER_WAIT_H_

#endif // _SYS_WAIT_H