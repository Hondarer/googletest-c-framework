#ifdef _WIN32

    #ifndef _OVERRIDE_SHELLAPI_H
        #define _OVERRIDE_SHELLAPI_H

        /* Windows SDK のヘッダーを include */
        #include <../um/shellapi.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SHELLAPI_H
        #include <mock_shellapi.h>
        #undef _IN_OVERRIDE_HEADER_SHELLAPI_H

    #endif // _OVERRIDE_SHELLAPI_H

#endif // _WIN32
