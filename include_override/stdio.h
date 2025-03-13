#ifndef _STDIO_H
/* 本物の include で define されるため、ここでは define しない */
/* #define _STDIO_H */

/* 本物を include */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "/usr/include/stdio.h"
#pragma GCC diagnostic pop

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDIO_H_
#include <mock_stdio.h>
#undef _IN_OVERRIDE_HEADER_STDIO_H_

#endif // _STDIO_H