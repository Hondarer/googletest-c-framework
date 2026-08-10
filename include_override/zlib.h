#ifndef _OVERRIDE_ZLIB_H
#define _OVERRIDE_ZLIB_H

/* 本物を include */
#include_next <zlib.h>

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_ZLIB_H
#include <mock_zlib.h>
#undef _IN_OVERRIDE_HEADER_ZLIB_H

#endif // _OVERRIDE_ZLIB_H
