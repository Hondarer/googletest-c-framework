#ifndef _WIN32

    #ifndef _OVERRIDE_SYS_MMAN_H
        #define _OVERRIDE_SYS_MMAN_H

        /* 本物を include */
        #include_next <sys/mman.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SYS_MMAN_H
        #include <sys/mock_mman.h>
        #undef _IN_OVERRIDE_HEADER_SYS_MMAN_H

    #endif // _OVERRIDE_SYS_MMAN_H

#endif // _WIN32
