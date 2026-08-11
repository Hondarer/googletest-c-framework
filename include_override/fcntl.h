#ifndef _OVERRIDE_FCNTL_H
#define _OVERRIDE_FCNTL_H

#ifdef _WIN32

    /* 本物を include */
    #pragma push_macro("_INC_FCNTL")
    #undef _INC_FCNTL
    #include <../ucrt/fcntl.h>
    #pragma pop_macro("_INC_FCNTL")

#else
    #include_next <fcntl.h>

    #define _IN_OVERRIDE_HEADER_FCNTL_H
    #include <mock_fcntl.h>
    #undef _IN_OVERRIDE_HEADER_FCNTL_H

#endif // _WIN32

#endif // _OVERRIDE_FCNTL_H
