#ifndef _WIN32

#ifndef _OVERRIDE_SYS_WAIT_H_
#define _OVERRIDE_SYS_WAIT_H_

/* 本物を include */
#include "/usr/include/sys/wait.h"

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_WAIT_H_
#include <sys/mock_wait.h>
#undef _IN_OVERRIDE_HEADER_WAIT_H_

#endif // _OVERRIDE_SYS_WAIT_H_

#endif // _WIN32
