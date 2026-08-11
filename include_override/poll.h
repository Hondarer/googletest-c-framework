#ifndef _OVERRIDE_POLL_H
#define _OVERRIDE_POLL_H

#ifndef _WIN32
    #include_next <poll.h>

    #define _IN_OVERRIDE_HEADER_POLL_H
    #include <mock_poll.h>
    #undef _IN_OVERRIDE_HEADER_POLL_H
#endif // _WIN32

#endif // _OVERRIDE_POLL_H
