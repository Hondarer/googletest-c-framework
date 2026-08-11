#ifndef _OVERRIDE_ARPA_INET_H
#define _OVERRIDE_ARPA_INET_H

#ifndef _WIN32
    #include_next <arpa/inet.h>

    #define _IN_OVERRIDE_HEADER_ARPA_INET_H
    #include <arpa/mock_inet.h>
    #undef _IN_OVERRIDE_HEADER_ARPA_INET_H
#endif // _WIN32

#endif // _OVERRIDE_ARPA_INET_H
