#ifndef _WIN32

    #ifndef _OVERRIDE_SYS_SOCKET_H
        #define _OVERRIDE_SYS_SOCKET_H

        /* 本物を include */
        #include_next <sys/socket.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SYS_SOCKET_H
        #include <sys/mock_socket.h>
        #undef _IN_OVERRIDE_HEADER_SYS_SOCKET_H

    #endif // _OVERRIDE_SYS_SOCKET_H

#endif // _WIN32
