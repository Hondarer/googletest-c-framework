#ifndef _STRING_H
/* 本物の include で define されるため、ここでは define しない */
/* #define _STRING_H */

/* 本物を include */
#include "/usr/include/string.h"

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STRING_H_
#include <mock_string.h>
#undef _IN_OVERRIDE_HEADER_STRING_H_

#endif // _STRING_H