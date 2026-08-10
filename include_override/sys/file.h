#ifndef _WIN32

    #ifndef _OVERRIDE_SYS_FILE_H
        #define _OVERRIDE_SYS_FILE_H

        /* 本物を include */
        #include_next <sys/file.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SYS_FILE_H
        #include <sys/mock_file.h>
        #undef _IN_OVERRIDE_HEADER_SYS_FILE_H

    #endif // _OVERRIDE_SYS_FILE_H

#endif // _WIN32
