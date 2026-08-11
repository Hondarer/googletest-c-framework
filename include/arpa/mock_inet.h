#ifndef _MOCK_ARPA_INET_H
#define _MOCK_ARPA_INET_H

#ifndef _WIN32

    /* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_ARPA_INET_H
        #include <arpa/inet.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_inet_pton(const char *, const int, const char *, int, const char *, void *);
    extern const char *mock_inet_ntop(const char *, const int, const char *, int, const void *, char *, socklen_t);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_ARPA_INET_H

        #define inet_pton(af, src, dst) mock_inet_pton(__FILE__, __LINE__, __func__, af, src, dst)
        #define inet_ntop(af, src, dst, size) \
            mock_inet_ntop(__FILE__, __LINE__, __func__, af, src, dst, size)

    #else // _IN_OVERRIDE_HEADER_ARPA_INET_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_inet_pton(const char *, const int, const char *, int, const char *, void *);
extern const char *delegate_real_inet_ntop(const char *, const int, const char *, int, const void *, char *, socklen_t);

class Mock_arpa_inet
{
  public:
    MOCK_METHOD(int, inet_pton, (const char *, const int, const char *, int, const char *, void *));
    MOCK_METHOD(const char *, inet_ntop, (const char *, const int, const char *, int, const void *, char *, socklen_t));

    Mock_arpa_inet();
    ~Mock_arpa_inet();
};

extern Mock_arpa_inet *_mock_arpa_inet;

    #endif // _IN_OVERRIDE_HEADER_ARPA_INET_H

#endif // _WIN32

#endif // _MOCK_ARPA_INET_H
