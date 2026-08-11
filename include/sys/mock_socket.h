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
    extern int mock_bind(const char *, const int, const char *, int, const struct sockaddr *, socklen_t);
    extern int mock_listen(const char *, const int, const char *, int, int);
    extern int mock_accept(const char *, const int, const char *, int, struct sockaddr *, socklen_t *);
    extern int mock_connect(const char *, const int, const char *, int, const struct sockaddr *, socklen_t);
    extern int mock_shutdown(const char *, const int, const char *, int, int);
    extern int mock_setsockopt(const char *, const int, const char *, int, int, int, const void *, socklen_t);
    extern int mock_getsockopt(const char *, const int, const char *, int, int, int, void *, socklen_t *);
    extern ssize_t mock_send(const char *, const int, const char *, int, const void *, size_t, int);
    extern ssize_t mock_recv(const char *, const int, const char *, int, void *, size_t, int);
    extern ssize_t mock_sendto(const char *, const int, const char *, int, const void *, size_t, int, const struct sockaddr *, socklen_t);
    extern ssize_t mock_recvfrom(const char *, const int, const char *, int, void *, size_t, int, struct sockaddr *, socklen_t *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_SOCKET_H

        #define socket(domain, type, protocol) \
            mock_socket(__FILE__, __LINE__, __func__, domain, type, protocol)
        #define bind(sockfd, addr, addrlen) \
            mock_bind(__FILE__, __LINE__, __func__, sockfd, addr, addrlen)
        #define listen(sockfd, backlog) \
            mock_listen(__FILE__, __LINE__, __func__, sockfd, backlog)
        #define accept(sockfd, addr, addrlen) \
            mock_accept(__FILE__, __LINE__, __func__, sockfd, addr, addrlen)
        #define connect(sockfd, addr, addrlen) \
            mock_connect(__FILE__, __LINE__, __func__, sockfd, addr, addrlen)
        #define shutdown(sockfd, how) \
            mock_shutdown(__FILE__, __LINE__, __func__, sockfd, how)
        #define setsockopt(sockfd, level, optname, optval, optlen) \
            mock_setsockopt(__FILE__, __LINE__, __func__, sockfd, level, optname, optval, optlen)
        #define getsockopt(sockfd, level, optname, optval, optlen) \
            mock_getsockopt(__FILE__, __LINE__, __func__, sockfd, level, optname, optval, optlen)
        #define send(sockfd, buf, len, flags) \
            mock_send(__FILE__, __LINE__, __func__, sockfd, buf, len, flags)
        #define recv(sockfd, buf, len, flags) \
            mock_recv(__FILE__, __LINE__, __func__, sockfd, buf, len, flags)
        #define sendto(sockfd, buf, len, flags, dest_addr, addrlen) \
            mock_sendto(__FILE__, __LINE__, __func__, sockfd, buf, len, flags, dest_addr, addrlen)
        #define recvfrom(sockfd, buf, len, flags, src_addr, addrlen) \
            mock_recvfrom(__FILE__, __LINE__, __func__, sockfd, buf, len, flags, src_addr, addrlen)

    #else // _IN_OVERRIDE_HEADER_SYS_SOCKET_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_socket(const char *, const int, const char *, int, int, int);
extern int delegate_real_bind(const char *, const int, const char *, int, const struct sockaddr *, socklen_t);
extern int delegate_real_listen(const char *, const int, const char *, int, int);
extern int delegate_real_accept(const char *, const int, const char *, int, struct sockaddr *, socklen_t *);
extern int delegate_real_connect(const char *, const int, const char *, int, const struct sockaddr *, socklen_t);
extern int delegate_real_shutdown(const char *, const int, const char *, int, int);
extern int delegate_real_setsockopt(const char *, const int, const char *, int, int, int, const void *, socklen_t);
extern int delegate_real_getsockopt(const char *, const int, const char *, int, int, int, void *, socklen_t *);
extern ssize_t delegate_real_send(const char *, const int, const char *, int, const void *, size_t, int);
extern ssize_t delegate_real_recv(const char *, const int, const char *, int, void *, size_t, int);
extern ssize_t delegate_real_sendto(const char *, const int, const char *, int, const void *, size_t, int, const struct sockaddr *, socklen_t);
extern ssize_t delegate_real_recvfrom(const char *, const int, const char *, int, void *, size_t, int, struct sockaddr *, socklen_t *);

class Mock_sys_socket
{
  public:
    MOCK_METHOD(int, socket, (const char *, const int, const char *, int, int, int));
    MOCK_METHOD(int, bind, (const char *, const int, const char *, int, const struct sockaddr *, socklen_t));
    MOCK_METHOD(int, listen, (const char *, const int, const char *, int, int));
    MOCK_METHOD(int, accept, (const char *, const int, const char *, int, struct sockaddr *, socklen_t *));
    MOCK_METHOD(int, connect, (const char *, const int, const char *, int, const struct sockaddr *, socklen_t));
    MOCK_METHOD(int, shutdown, (const char *, const int, const char *, int, int));
    MOCK_METHOD(int, setsockopt, (const char *, const int, const char *, int, int, int, const void *, socklen_t));
    MOCK_METHOD(int, getsockopt, (const char *, const int, const char *, int, int, int, void *, socklen_t *));
    MOCK_METHOD(ssize_t, send, (const char *, const int, const char *, int, const void *, size_t, int));
    MOCK_METHOD(ssize_t, recv, (const char *, const int, const char *, int, void *, size_t, int));
    MOCK_METHOD(ssize_t, sendto, (const char *, const int, const char *, int, const void *, size_t, int, const struct sockaddr *, socklen_t));
    MOCK_METHOD(ssize_t, recvfrom, (const char *, const int, const char *, int, void *, size_t, int, struct sockaddr *, socklen_t *));

    Mock_sys_socket();
    ~Mock_sys_socket();
};

extern Mock_sys_socket *_mock_sys_socket;

    #endif // _IN_OVERRIDE_HEADER_SYS_SOCKET_H

#endif // _WIN32

#endif // _MOCK_SYS_SOCKET_H
