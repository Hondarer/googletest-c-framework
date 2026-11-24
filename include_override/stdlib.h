#ifndef _OVERRIDE_STDLIB_H
#define _OVERRIDE_STDLIB_H

/* libc のヘッダーを include */
#ifndef _WIN32
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Wpadded"
    /* unistd.h override で mkstemp と mkostemp がマクロ化されていると stdlib.h の宣言が壊れるため退避 */
    #pragma push_macro("mkstemp")
    #pragma push_macro("mkostemp")
    #undef mkstemp
    #undef mkostemp
    #include_next <stdlib.h>
    #pragma pop_macro("mkostemp")
    #pragma pop_macro("mkstemp")
    #pragma GCC diagnostic pop
#else // _WIN32
    #pragma push_macro("_INC_STDLIB")
    #undef _INC_STDLIB
    #include <../ucrt/stdlib.h>
    #pragma pop_macro("_INC_STDLIB")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDLIB_H
#include <mock_stdlib.h>
#undef _IN_OVERRIDE_HEADER_STDLIB_H

#endif // _OVERRIDE_STDLIB_H
