#ifndef _OVERRIDE_NETDB_H
#define _OVERRIDE_NETDB_H

#ifndef _WIN32
    #include_next <netdb.h>

    #define _IN_OVERRIDE_HEADER_NETDB_H
    #include <mock_netdb.h>
    #undef _IN_OVERRIDE_HEADER_NETDB_H
#endif // _WIN32

#endif // _OVERRIDE_NETDB_H
