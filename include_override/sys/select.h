#ifndef _WIN32

    #ifndef _OVERRIDE_SYS_SELECT_H
        #define _OVERRIDE_SYS_SELECT_H

        /* 本物を include */
        #include_next <sys/select.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SYS_SELECT_H
        #include <sys/mock_select.h>
        #undef _IN_OVERRIDE_HEADER_SYS_SELECT_H

    #endif // _OVERRIDE_SYS_SELECT_H

#endif // _WIN32
