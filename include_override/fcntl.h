#ifndef _OVERRIDE_FCNTL_H
#define _OVERRIDE_FCNTL_H

#ifndef _WIN32
    #include_next <fcntl.h>

    #define _IN_OVERRIDE_HEADER_FCNTL_H
    #include <mock_fcntl.h>
    #undef _IN_OVERRIDE_HEADER_FCNTL_H
#endif // _WIN32

#endif // _OVERRIDE_FCNTL_H
