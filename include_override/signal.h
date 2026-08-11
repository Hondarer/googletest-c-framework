#ifndef _OVERRIDE_SIGNAL_H
#define _OVERRIDE_SIGNAL_H

#ifdef _WIN32

    /* 本物を include */
    #pragma push_macro("_INC_SIGNAL")
    #undef _INC_SIGNAL
    #include <../ucrt/signal.h>
    #pragma pop_macro("_INC_SIGNAL")

#else
    #include_next <signal.h>

    #define _IN_OVERRIDE_HEADER_SIGNAL_H
    #include <mock_signal.h>
    #undef _IN_OVERRIDE_HEADER_SIGNAL_H

#endif // _WIN32

#endif // _OVERRIDE_SIGNAL_H
