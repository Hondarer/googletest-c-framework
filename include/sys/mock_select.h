#ifndef _MOCK_SYS_SELECT_H
#define _MOCK_SYS_SELECT_H

#ifndef _WIN32

    #include <sys/select.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_select(const char *, const int, const char *, int, fd_set *, fd_set *, fd_set *,
                           struct timeval *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_SELECT_H

        #define select(nfds, readfds, writefds, exceptfds, timeout) \
            mock_select(__FILE__, __LINE__, __func__, nfds, readfds, writefds, exceptfds, timeout)

    #else // _IN_OVERRIDE_HEADER_SYS_SELECT_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_select(const char *, const int, const char *, int, fd_set *, fd_set *, fd_set *,
                                struct timeval *);

class Mock_sys_select
{
  public:
    MOCK_METHOD(int, select,
                (const char *, const int, const char *, int, fd_set *, fd_set *, fd_set *, struct timeval *));

    Mock_sys_select();
    ~Mock_sys_select();
};

extern Mock_sys_select *_mock_sys_select;

    #endif // _IN_OVERRIDE_HEADER_SYS_SELECT_H

#endif // _WIN32

#endif // _MOCK_SYS_SELECT_H
