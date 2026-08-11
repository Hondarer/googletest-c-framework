#ifndef _MOCK_IOCTL_H
#define _MOCK_IOCTL_H

#ifndef _WIN32

    #include <sys/ioctl.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_ioctl(const char *, const int, const char *, int, unsigned long, void *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_IOCTL_H
        #define ioctl(fd, request, arg) mock_ioctl(__FILE__, __LINE__, __func__, fd, request, arg)
    #else // _IN_OVERRIDE_HEADER_SYS_IOCTL_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_ioctl(const char *, const int, const char *, int, unsigned long, void *);

class Mock_ioctl
{
  public:
    MOCK_METHOD(int, ioctl, (const char *, const int, const char *, int, unsigned long, void *));

    Mock_ioctl();
    ~Mock_ioctl();
};

extern Mock_ioctl *_mock_ioctl;

    #endif // _IN_OVERRIDE_HEADER_SYS_IOCTL_H

#endif // _WIN32

#endif // _MOCK_IOCTL_H
