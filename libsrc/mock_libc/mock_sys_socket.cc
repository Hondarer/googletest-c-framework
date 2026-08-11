#include <test_com.h>
#include <sys/mock_socket.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_socket *_mock_sys_socket = nullptr;

Mock_sys_socket::Mock_sys_socket()
{
    ON_CALL(*this, socket(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_socket));
    ON_CALL(*this, bind(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_bind));
    ON_CALL(*this, listen(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_listen));
    ON_CALL(*this, accept(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_accept));
    ON_CALL(*this, connect(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_connect));
    ON_CALL(*this, shutdown(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_shutdown));
    ON_CALL(*this, setsockopt(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_setsockopt));
    ON_CALL(*this, getsockopt(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_getsockopt));
    ON_CALL(*this, send(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_send));
    ON_CALL(*this, recv(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_recv));
    ON_CALL(*this, sendto(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sendto));
    ON_CALL(*this, recvfrom(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_recvfrom));

    _mock_sys_socket = this;
}

Mock_sys_socket::~Mock_sys_socket()
{
    _mock_sys_socket = nullptr;
}

/* 全関数で同一のトレース出力を行うため、呼び出し記録をマクロへ切り出す。 */
    #define TRACE_SOCKET_CALL(name) \
        do \
        { \
            if (getTraceLevel() > TRACE_NONE) \
            { \
                printf("  > " #name); \
                if (getTraceLevel() >= TRACE_DETAIL) \
                { \
                    printf(" from %s:%d\n", file, line); \
                } \
                else \
                { \
                    printf("\n"); \
                } \
            } \
        } while (0)

int delegate_real_socket(const char *file, const int line, const char *func, int domain, int type, int protocol)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return socket(domain, type, protocol);
}

int mock_socket(const char *file, const int line, const char *func, int domain, int type, int protocol)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->socket(file, line, func, domain, type, protocol);
    }
    else
    {
        result = delegate_real_socket(file, line, func, domain, type, protocol);
    }

    TRACE_SOCKET_CALL(socket);

    return result;
}

int delegate_real_bind(const char *file, const int line, const char *func, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return bind(sockfd, addr, addrlen);
}

int mock_bind(const char *file, const int line, const char *func, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->bind(file, line, func, sockfd, addr, addrlen);
    }
    else
    {
        result = delegate_real_bind(file, line, func, sockfd, addr, addrlen);
    }

    TRACE_SOCKET_CALL(bind);

    return result;
}

int delegate_real_listen(const char *file, const int line, const char *func, int sockfd, int backlog)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return listen(sockfd, backlog);
}

int mock_listen(const char *file, const int line, const char *func, int sockfd, int backlog)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->listen(file, line, func, sockfd, backlog);
    }
    else
    {
        result = delegate_real_listen(file, line, func, sockfd, backlog);
    }

    TRACE_SOCKET_CALL(listen);

    return result;
}

int delegate_real_accept(const char *file, const int line, const char *func, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return accept(sockfd, addr, addrlen);
}

int mock_accept(const char *file, const int line, const char *func, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->accept(file, line, func, sockfd, addr, addrlen);
    }
    else
    {
        result = delegate_real_accept(file, line, func, sockfd, addr, addrlen);
    }

    TRACE_SOCKET_CALL(accept);

    return result;
}

int delegate_real_connect(const char *file, const int line, const char *func, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return connect(sockfd, addr, addrlen);
}

int mock_connect(const char *file, const int line, const char *func, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->connect(file, line, func, sockfd, addr, addrlen);
    }
    else
    {
        result = delegate_real_connect(file, line, func, sockfd, addr, addrlen);
    }

    TRACE_SOCKET_CALL(connect);

    return result;
}

int delegate_real_shutdown(const char *file, const int line, const char *func, int sockfd, int how)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return shutdown(sockfd, how);
}

int mock_shutdown(const char *file, const int line, const char *func, int sockfd, int how)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->shutdown(file, line, func, sockfd, how);
    }
    else
    {
        result = delegate_real_shutdown(file, line, func, sockfd, how);
    }

    TRACE_SOCKET_CALL(shutdown);

    return result;
}

int delegate_real_setsockopt(const char *file, const int line, const char *func, int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return setsockopt(sockfd, level, optname, optval, optlen);
}

int mock_setsockopt(const char *file, const int line, const char *func, int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->setsockopt(file, line, func, sockfd, level, optname, optval, optlen);
    }
    else
    {
        result = delegate_real_setsockopt(file, line, func, sockfd, level, optname, optval, optlen);
    }

    TRACE_SOCKET_CALL(setsockopt);

    return result;
}

int delegate_real_getsockopt(const char *file, const int line, const char *func, int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getsockopt(sockfd, level, optname, optval, optlen);
}

int mock_getsockopt(const char *file, const int line, const char *func, int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    int result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->getsockopt(file, line, func, sockfd, level, optname, optval, optlen);
    }
    else
    {
        result = delegate_real_getsockopt(file, line, func, sockfd, level, optname, optval, optlen);
    }

    TRACE_SOCKET_CALL(getsockopt);

    return result;
}

ssize_t delegate_real_send(const char *file, const int line, const char *func, int sockfd, const void *buf, size_t len, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return send(sockfd, buf, len, flags);
}

ssize_t mock_send(const char *file, const int line, const char *func, int sockfd, const void *buf, size_t len, int flags)
{
    ssize_t result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->send(file, line, func, sockfd, buf, len, flags);
    }
    else
    {
        result = delegate_real_send(file, line, func, sockfd, buf, len, flags);
    }

    TRACE_SOCKET_CALL(send);

    return result;
}

ssize_t delegate_real_recv(const char *file, const int line, const char *func, int sockfd, void *buf, size_t len, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return recv(sockfd, buf, len, flags);
}

ssize_t mock_recv(const char *file, const int line, const char *func, int sockfd, void *buf, size_t len, int flags)
{
    ssize_t result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->recv(file, line, func, sockfd, buf, len, flags);
    }
    else
    {
        result = delegate_real_recv(file, line, func, sockfd, buf, len, flags);
    }

    TRACE_SOCKET_CALL(recv);

    return result;
}

ssize_t delegate_real_sendto(const char *file, const int line, const char *func, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}

ssize_t mock_sendto(const char *file, const int line, const char *func, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    ssize_t result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->sendto(file, line, func, sockfd, buf, len, flags, dest_addr, addrlen);
    }
    else
    {
        result = delegate_real_sendto(file, line, func, sockfd, buf, len, flags, dest_addr, addrlen);
    }

    TRACE_SOCKET_CALL(sendto);

    return result;
}

ssize_t delegate_real_recvfrom(const char *file, const int line, const char *func, int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

ssize_t mock_recvfrom(const char *file, const int line, const char *func, int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t result;

    if (_mock_sys_socket != nullptr)
    {
        result = _mock_sys_socket->recvfrom(file, line, func, sockfd, buf, len, flags, src_addr, addrlen);
    }
    else
    {
        result = delegate_real_recvfrom(file, line, func, sockfd, buf, len, flags, src_addr, addrlen);
    }

    TRACE_SOCKET_CALL(recvfrom);

    return result;
}

#endif // _WIN32
