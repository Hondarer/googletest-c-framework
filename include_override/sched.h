#ifndef _WIN32

    #ifndef _OVERRIDE_SCHED_H
        #define _OVERRIDE_SCHED_H

        /* libc のヘッダーを include */
        #include_next <sched.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SCHED_H
        #include <mock_sched.h>
        #undef _IN_OVERRIDE_HEADER_SCHED_H

    #endif // _OVERRIDE_SCHED_H

#endif // _WIN32
