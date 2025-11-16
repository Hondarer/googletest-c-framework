#ifndef _WIN32

#ifndef _OVERRIDE_UNISTD_H_
#define _OVERRIDE_UNISTD_H_

/* 本物を include */
#include "/usr/include/unistd.h"

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_UNISTD_H_
#include <mock_unistd.h>
#undef _IN_OVERRIDE_HEADER_UNISTD_H_

#endif // _OVERRIDE_UNISTD_H_

#endif // _WIN32
