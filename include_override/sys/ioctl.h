#ifndef _OVERRIDE_SYS_IOCTL_H
#define _OVERRIDE_SYS_IOCTL_H

#ifndef _WIN32
    #include_next <sys/ioctl.h>

    #define _IN_OVERRIDE_HEADER_SYS_IOCTL_H
    #include <mock_ioctl.h>
    #undef _IN_OVERRIDE_HEADER_SYS_IOCTL_H
#endif // _WIN32

#endif // _OVERRIDE_SYS_IOCTL_H
