#ifndef _OVERRIDE_SIGNAL_H
#define _OVERRIDE_SIGNAL_H

#ifndef _WIN32
    #include_next <signal.h>

    #define _IN_OVERRIDE_HEADER_SIGNAL_H
    #include <mock_signal.h>
    #undef _IN_OVERRIDE_HEADER_SIGNAL_H
#endif // _WIN32

#endif // _OVERRIDE_SIGNAL_H
