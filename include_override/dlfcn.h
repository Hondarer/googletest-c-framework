#ifndef _WIN32

    #ifndef _OVERRIDE_DLFCN_H
        #define _OVERRIDE_DLFCN_H

        /* 本物を include */
        #include_next <dlfcn.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_DLFCN_H
        #include <mock_dlfcn.h>
        #undef _IN_OVERRIDE_HEADER_DLFCN_H

    #endif // _OVERRIDE_DLFCN_H

#endif // _WIN32
