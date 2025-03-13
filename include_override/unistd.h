#ifndef _UNISTD_H
/* 本物の include で define されるため、ここでは define しない */
/* #define _UNISTD_H */

/* 本物を include */
#include "/usr/include/unistd.h"

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_UNISTD_H_
#include <mock_unistd.h>
#undef _IN_OVERRIDE_HEADER_UNISTD_H_

#endif // _UNISTD_H