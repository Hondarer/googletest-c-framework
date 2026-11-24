#ifndef _MOCK_EVENTFD_H
#define _MOCK_EVENTFD_H

#ifndef _WIN32

    /* override 経由の場合、libc のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_EVENTFD_H
        #include <sys/eventfd.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_eventfd(const char *, const int, const char *, unsigned int, int);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_EVENTFD_H

        #define eventfd(initial_value, flags) mock_eventfd(__FILE__, __LINE__, __func__, initial_value, flags)

    #else // _IN_OVERRIDE_HEADER_EVENTFD_H

        #include <gmock/gmock.h>

extern int delegate_real_eventfd(const char *, const int, const char *, unsigned int, int);

class Mock_eventfd
{
  public:
    MOCK_METHOD(int, eventfd, (const char *, const int, const char *, unsigned int, int));

    Mock_eventfd();
    virtual ~Mock_eventfd();
};

extern Mock_eventfd *_mock_eventfd;

    #endif // _IN_OVERRIDE_HEADER_EVENTFD_H

#endif // _WIN32

#endif // _MOCK_EVENTFD_H
