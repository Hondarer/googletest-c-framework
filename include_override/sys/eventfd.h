#ifndef _WIN32

    #ifndef _OVERRIDE_SYS_EVENTFD_H
        #define _OVERRIDE_SYS_EVENTFD_H

        #include_next <sys/eventfd.h>

        #define _IN_OVERRIDE_HEADER_EVENTFD_H
        #include <sys/mock_eventfd.h>
        #undef _IN_OVERRIDE_HEADER_EVENTFD_H

    #endif // _OVERRIDE_SYS_EVENTFD_H

#endif // _WIN32
