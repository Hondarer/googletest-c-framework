#ifdef _WIN32

    #ifndef _OVERRIDE_DIRECT_H
        #define _OVERRIDE_DIRECT_H

        /* libc のヘッダーを include */
        #pragma push_macro("_INC_DIRECT")
        #undef _INC_DIRECT
        #include <../ucrt/direct.h>
        #pragma pop_macro("_INC_DIRECT")

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_DIRECT_H
        #include <mock_direct.h>
        #undef _IN_OVERRIDE_HEADER_DIRECT_H

    #endif // _OVERRIDE_DIRECT_H

#endif // _WIN32
