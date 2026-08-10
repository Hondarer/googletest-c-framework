#ifndef _MOCK_SYS_SOCKET_H
#define _MOCK_SYS_SOCKET_H

#ifndef _WIN32

    /* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_SYS_SOCKET_H
        #include <sys/socket.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_socket(const char *, const int, const char *, int, int, int);
    extern ssize_t mock_sendto(const char *, const int, const char *, int, const void *, size_t, int, const struct sockaddr *, socklen_t);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_SOCKET_H

        #define socket(domain, type, protocol) \
            mock_socket(__FILE__, __LINE__, __func__, domain, type, protocol)
        #define sendto(sockfd, buf, len, flags, dest_addr, addrlen) \
            mock_sendto(__FILE__, __LINE__, __func__, sockfd, buf, len, flags, dest_addr, addrlen)

    #else // _IN_OVERRIDE_HEADER_SYS_SOCKET_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_socket(const char *, const int, const char *, int, int, int);
extern ssize_t delegate_real_sendto(const char *, const int, const char *, int, const void *, size_t, int, const struct sockaddr *, socklen_t);

class Mock_sys_socket
{
  public:
    MOCK_METHOD(int, socket, (const char *, const int, const char *, int, int, int));
    MOCK_METHOD(ssize_t, sendto, (const char *, const int, const char *, int, const void *, size_t, int, const struct sockaddr *, socklen_t));

    Mock_sys_socket();
    ~Mock_sys_socket();
};

extern Mock_sys_socket *_mock_sys_socket;

    #endif // _IN_OVERRIDE_HEADER_SYS_SOCKET_H

#endif // _WIN32

#endif // _MOCK_SYS_SOCKET_H
