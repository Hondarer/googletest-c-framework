#ifndef _WIN32

    #ifndef _OVERRIDE_UNISTD_H
        #define _OVERRIDE_UNISTD_H

        /* signal.h は kill() を宣言するため、kill の mock macro より先に読み込む。 */
        #include_next <signal.h>

        /* 本物を include */
        #include_next <unistd.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_UNISTD_H
        #include <mock_unistd.h>
        #undef _IN_OVERRIDE_HEADER_UNISTD_H

    #endif // _OVERRIDE_UNISTD_H

#endif // _WIN32
