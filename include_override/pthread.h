#ifndef _WIN32

    #ifndef _OVERRIDE_PTHREAD_H
        #define _OVERRIDE_PTHREAD_H

        /* 本物を include */
        #include_next <pthread.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_PTHREAD_H
        #include <mock_pthread.h>
        #undef _IN_OVERRIDE_HEADER_PTHREAD_H

    #endif // _OVERRIDE_PTHREAD_H

#endif // _WIN32
