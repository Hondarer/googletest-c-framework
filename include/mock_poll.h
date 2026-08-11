#ifndef _MOCK_POLL_H
#define _MOCK_POLL_H

#ifndef _WIN32

    /* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_POLL_H
        #include <poll.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_poll(const char *, const int, const char *, struct pollfd *, nfds_t, int);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_POLL_H

        #define poll(fds, nfds, timeout) mock_poll(__FILE__, __LINE__, __func__, fds, nfds, timeout)

    #else // _IN_OVERRIDE_HEADER_POLL_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_poll(const char *, const int, const char *, struct pollfd *, nfds_t, int);

class Mock_poll
{
  public:
    MOCK_METHOD(int, poll, (const char *, const int, const char *, struct pollfd *, nfds_t, int));

    Mock_poll();
    ~Mock_poll();
};

extern Mock_poll *_mock_poll;

    #endif // _IN_OVERRIDE_HEADER_POLL_H

#endif // _WIN32

#endif // _MOCK_POLL_H
