#ifndef _MOCK_NETDB_H
#define _MOCK_NETDB_H

#ifndef _WIN32

    /* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_NETDB_H
        #include <netdb.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_getaddrinfo(const char *, const int, const char *, const char *, const char *, const struct addrinfo *, struct addrinfo **);
    extern void mock_freeaddrinfo(const char *, const int, const char *, struct addrinfo *);
    extern const char *mock_gai_strerror(const char *, const int, const char *, int);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_NETDB_H

        #define getaddrinfo(node, service, hints, res) \
            mock_getaddrinfo(__FILE__, __LINE__, __func__, node, service, hints, res)
        #define freeaddrinfo(res)     mock_freeaddrinfo(__FILE__, __LINE__, __func__, res)
        #define gai_strerror(errcode) mock_gai_strerror(__FILE__, __LINE__, __func__, errcode)

    #else // _IN_OVERRIDE_HEADER_NETDB_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_getaddrinfo(const char *, const int, const char *, const char *, const char *, const struct addrinfo *, struct addrinfo **);
extern void delegate_real_freeaddrinfo(const char *, const int, const char *, struct addrinfo *);
extern const char *delegate_real_gai_strerror(const char *, const int, const char *, int);

class Mock_netdb
{
  public:
    MOCK_METHOD(int, getaddrinfo, (const char *, const int, const char *, const char *, const char *, const struct addrinfo *, struct addrinfo **));
    MOCK_METHOD(void, freeaddrinfo, (const char *, const int, const char *, struct addrinfo *));
    MOCK_METHOD(const char *, gai_strerror, (const char *, const int, const char *, int));

    Mock_netdb();
    ~Mock_netdb();
};

extern Mock_netdb *_mock_netdb;

    #endif // _IN_OVERRIDE_HEADER_NETDB_H

#endif // _WIN32

#endif // _MOCK_NETDB_H
