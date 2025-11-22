#ifndef _WIN32

#ifndef _OVERRIDE_SYS_WAIT_H
#define _OVERRIDE_SYS_WAIT_H

/* 本物を include */
#include_next <sys/wait.h>

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_WAIT_H
#include <sys/mock_wait.h>
#undef _IN_OVERRIDE_HEADER_WAIT_H

#endif // _OVERRIDE_SYS_WAIT_H

#endif // _WIN32
