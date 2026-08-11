#include <mock_winsock.h>
#include <test_com.h>

#ifdef _WIN32

    #pragma comment(lib, "ws2_32.lib")

using namespace testing;

Mock_winsock *_mock_winsock = nullptr;

Mock_winsock::Mock_winsock()
{
    ON_CALL(*this, WSAStartup(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_WSAStartup));
    ON_CALL(*this, WSACleanup(_, _, _)).WillByDefault(Invoke(delegate_real_WSACleanup));
    ON_CALL(*this, WSAGetLastError(_, _, _)).WillByDefault(Invoke(delegate_real_WSAGetLastError));
    ON_CALL(*this, WSAPoll(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_WSAPoll));
    ON_CALL(*this, socket(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_socket));
    ON_CALL(*this, bind(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_bind));
    ON_CALL(*this, listen(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_listen));
    ON_CALL(*this, accept(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_accept));
    ON_CALL(*this, connect(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_connect));
    ON_CALL(*this, shutdown(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_shutdown));
    ON_CALL(*this, closesocket(_, _, _, _)).WillByDefault(Invoke(delegate_real_closesocket));
    ON_CALL(*this, ioctlsocket(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_ioctlsocket));
    ON_CALL(*this, setsockopt(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_setsockopt));
    ON_CALL(*this, getsockopt(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_getsockopt));
    ON_CALL(*this, send(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_send));
    ON_CALL(*this, recv(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_recv));
    ON_CALL(*this, sendto(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sendto));
    ON_CALL(*this, recvfrom(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_recvfrom));
    ON_CALL(*this, inet_pton(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_inet_pton));
    ON_CALL(*this, inet_ntop(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_inet_ntop));
    ON_CALL(*this, getaddrinfo(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_getaddrinfo));
    ON_CALL(*this, freeaddrinfo(_, _, _, _)).WillByDefault(Invoke(delegate_real_freeaddrinfo));

    _mock_winsock = this;
}

Mock_winsock::~Mock_winsock()
{
    _mock_winsock = nullptr;
}

/* 全関数で同一のトレース出力を行うため、呼び出し記録をマクロへ切り出す。 */
    #define TRACE_WINSOCK_CALL(name) \
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

int delegate_real_WSAStartup(const char *file, const int line, const char *func, WORD version_required, LPWSADATA wsa_data)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WSAStartup(version_required, wsa_data);
}

int mock_WSAStartup(const char *file, const int line, const char *func, WORD version_required, LPWSADATA wsa_data)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->WSAStartup(file, line, func, version_required, wsa_data);
    }
    else
    {
        result = delegate_real_WSAStartup(file, line, func, version_required, wsa_data);
    }

    TRACE_WINSOCK_CALL(WSAStartup);

    return result;
}

int delegate_real_WSACleanup(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WSACleanup();
}

int mock_WSACleanup(const char *file, const int line, const char *func)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->WSACleanup(file, line, func);
    }
    else
    {
        result = delegate_real_WSACleanup(file, line, func);
    }

    TRACE_WINSOCK_CALL(WSACleanup);

    return result;
}

int delegate_real_WSAGetLastError(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WSAGetLastError();
}

int mock_WSAGetLastError(const char *file, const int line, const char *func)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->WSAGetLastError(file, line, func);
    }
    else
    {
        result = delegate_real_WSAGetLastError(file, line, func);
    }

    /* WSAGetLastError はエラー経路で頻繁に呼ばれるため、トレースは詳細レベルに限定する。 */
    if (getTraceLevel() >= TRACE_DETAIL)
    {
        printf("  > WSAGetLastError from %s:%d\n", file, line);
    }

    return result;
}

int delegate_real_WSAPoll(const char *file, const int line, const char *func, LPWSAPOLLFD fd_array, ULONG fds, INT timeout)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WSAPoll(fd_array, fds, timeout);
}

int mock_WSAPoll(const char *file, const int line, const char *func, LPWSAPOLLFD fd_array, ULONG fds, INT timeout)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->WSAPoll(file, line, func, fd_array, fds, timeout);
    }
    else
    {
        result = delegate_real_WSAPoll(file, line, func, fd_array, fds, timeout);
    }

    TRACE_WINSOCK_CALL(WSAPoll);

    return result;
}

SOCKET delegate_real_socket(const char *file, const int line, const char *func, int af, int type, int protocol)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return socket(af, type, protocol);
}

SOCKET mock_socket(const char *file, const int line, const char *func, int af, int type, int protocol)
{
    SOCKET result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->socket(file, line, func, af, type, protocol);
    }
    else
    {
        result = delegate_real_socket(file, line, func, af, type, protocol);
    }

    TRACE_WINSOCK_CALL(socket);

    return result;
}

int delegate_real_bind(const char *file, const int line, const char *func, SOCKET s, const struct sockaddr *name, int namelen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return bind(s, name, namelen);
}

int mock_bind(const char *file, const int line, const char *func, SOCKET s, const struct sockaddr *name, int namelen)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->bind(file, line, func, s, name, namelen);
    }
    else
    {
        result = delegate_real_bind(file, line, func, s, name, namelen);
    }

    TRACE_WINSOCK_CALL(bind);

    return result;
}

int delegate_real_listen(const char *file, const int line, const char *func, SOCKET s, int backlog)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return listen(s, backlog);
}

int mock_listen(const char *file, const int line, const char *func, SOCKET s, int backlog)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->listen(file, line, func, s, backlog);
    }
    else
    {
        result = delegate_real_listen(file, line, func, s, backlog);
    }

    TRACE_WINSOCK_CALL(listen);

    return result;
}

SOCKET delegate_real_accept(const char *file, const int line, const char *func, SOCKET s, struct sockaddr *addr, int *addrlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return accept(s, addr, addrlen);
}

SOCKET mock_accept(const char *file, const int line, const char *func, SOCKET s, struct sockaddr *addr, int *addrlen)
{
    SOCKET result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->accept(file, line, func, s, addr, addrlen);
    }
    else
    {
        result = delegate_real_accept(file, line, func, s, addr, addrlen);
    }

    TRACE_WINSOCK_CALL(accept);

    return result;
}

int delegate_real_connect(const char *file, const int line, const char *func, SOCKET s, const struct sockaddr *name, int namelen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return connect(s, name, namelen);
}

int mock_connect(const char *file, const int line, const char *func, SOCKET s, const struct sockaddr *name, int namelen)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->connect(file, line, func, s, name, namelen);
    }
    else
    {
        result = delegate_real_connect(file, line, func, s, name, namelen);
    }

    TRACE_WINSOCK_CALL(connect);

    return result;
}

int delegate_real_shutdown(const char *file, const int line, const char *func, SOCKET s, int how)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return shutdown(s, how);
}

int mock_shutdown(const char *file, const int line, const char *func, SOCKET s, int how)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->shutdown(file, line, func, s, how);
    }
    else
    {
        result = delegate_real_shutdown(file, line, func, s, how);
    }

    TRACE_WINSOCK_CALL(shutdown);

    return result;
}

int delegate_real_closesocket(const char *file, const int line, const char *func, SOCKET s)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return closesocket(s);
}

int mock_closesocket(const char *file, const int line, const char *func, SOCKET s)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->closesocket(file, line, func, s);
    }
    else
    {
        result = delegate_real_closesocket(file, line, func, s);
    }

    TRACE_WINSOCK_CALL(closesocket);

    return result;
}

int delegate_real_ioctlsocket(const char *file, const int line, const char *func, SOCKET s, long cmd, u_long *argp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ioctlsocket(s, cmd, argp);
}

int mock_ioctlsocket(const char *file, const int line, const char *func, SOCKET s, long cmd, u_long *argp)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->ioctlsocket(file, line, func, s, cmd, argp);
    }
    else
    {
        result = delegate_real_ioctlsocket(file, line, func, s, cmd, argp);
    }

    TRACE_WINSOCK_CALL(ioctlsocket);

    return result;
}

int delegate_real_setsockopt(const char *file, const int line, const char *func, SOCKET s, int level, int optname, const char *optval, int optlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return setsockopt(s, level, optname, optval, optlen);
}

int mock_setsockopt(const char *file, const int line, const char *func, SOCKET s, int level, int optname, const char *optval, int optlen)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->setsockopt(file, line, func, s, level, optname, optval, optlen);
    }
    else
    {
        result = delegate_real_setsockopt(file, line, func, s, level, optname, optval, optlen);
    }

    TRACE_WINSOCK_CALL(setsockopt);

    return result;
}

int delegate_real_getsockopt(const char *file, const int line, const char *func, SOCKET s, int level, int optname, char *optval, int *optlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getsockopt(s, level, optname, optval, optlen);
}

int mock_getsockopt(const char *file, const int line, const char *func, SOCKET s, int level, int optname, char *optval, int *optlen)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->getsockopt(file, line, func, s, level, optname, optval, optlen);
    }
    else
    {
        result = delegate_real_getsockopt(file, line, func, s, level, optname, optval, optlen);
    }

    TRACE_WINSOCK_CALL(getsockopt);

    return result;
}

int delegate_real_send(const char *file, const int line, const char *func, SOCKET s, const char *buf, int len, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return send(s, buf, len, flags);
}

int mock_send(const char *file, const int line, const char *func, SOCKET s, const char *buf, int len, int flags)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->send(file, line, func, s, buf, len, flags);
    }
    else
    {
        result = delegate_real_send(file, line, func, s, buf, len, flags);
    }

    TRACE_WINSOCK_CALL(send);

    return result;
}

int delegate_real_recv(const char *file, const int line, const char *func, SOCKET s, char *buf, int len, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return recv(s, buf, len, flags);
}

int mock_recv(const char *file, const int line, const char *func, SOCKET s, char *buf, int len, int flags)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->recv(file, line, func, s, buf, len, flags);
    }
    else
    {
        result = delegate_real_recv(file, line, func, s, buf, len, flags);
    }

    TRACE_WINSOCK_CALL(recv);

    return result;
}

int delegate_real_sendto(const char *file, const int line, const char *func, SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return sendto(s, buf, len, flags, to, tolen);
}

int mock_sendto(const char *file, const int line, const char *func, SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->sendto(file, line, func, s, buf, len, flags, to, tolen);
    }
    else
    {
        result = delegate_real_sendto(file, line, func, s, buf, len, flags, to, tolen);
    }

    TRACE_WINSOCK_CALL(sendto);

    return result;
}

int delegate_real_recvfrom(const char *file, const int line, const char *func, SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return recvfrom(s, buf, len, flags, from, fromlen);
}

int mock_recvfrom(const char *file, const int line, const char *func, SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
    int result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->recvfrom(file, line, func, s, buf, len, flags, from, fromlen);
    }
    else
    {
        result = delegate_real_recvfrom(file, line, func, s, buf, len, flags, from, fromlen);
    }

    TRACE_WINSOCK_CALL(recvfrom);

    return result;
}

INT delegate_real_inet_pton(const char *file, const int line, const char *func, INT family, PCSTR addr_string, PVOID addr_buf)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return inet_pton(family, addr_string, addr_buf);
}

INT mock_inet_pton(const char *file, const int line, const char *func, INT family, PCSTR addr_string, PVOID addr_buf)
{
    INT result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->inet_pton(file, line, func, family, addr_string, addr_buf);
    }
    else
    {
        result = delegate_real_inet_pton(file, line, func, family, addr_string, addr_buf);
    }

    TRACE_WINSOCK_CALL(inet_pton);

    return result;
}

PCSTR delegate_real_inet_ntop(const char *file, const int line, const char *func, INT family, const void *addr, PSTR string_buf, size_t string_buf_size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return inet_ntop(family, addr, string_buf, string_buf_size);
}

PCSTR mock_inet_ntop(const char *file, const int line, const char *func, INT family, const void *addr, PSTR string_buf, size_t string_buf_size)
{
    PCSTR result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->inet_ntop(file, line, func, family, addr, string_buf, string_buf_size);
    }
    else
    {
        result = delegate_real_inet_ntop(file, line, func, family, addr, string_buf, string_buf_size);
    }

    TRACE_WINSOCK_CALL(inet_ntop);

    return result;
}

INT delegate_real_getaddrinfo(const char *file, const int line, const char *func, PCSTR node_name, PCSTR service_name, const ADDRINFOA *hints, PADDRINFOA *result_out)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getaddrinfo(node_name, service_name, hints, result_out);
}

INT mock_getaddrinfo(const char *file, const int line, const char *func, PCSTR node_name, PCSTR service_name, const ADDRINFOA *hints, PADDRINFOA *result_out)
{
    INT result;

    if (_mock_winsock != nullptr)
    {
        result = _mock_winsock->getaddrinfo(file, line, func, node_name, service_name, hints, result_out);
    }
    else
    {
        result = delegate_real_getaddrinfo(file, line, func, node_name, service_name, hints, result_out);
    }

    TRACE_WINSOCK_CALL(getaddrinfo);

    return result;
}

void delegate_real_freeaddrinfo(const char *file, const int line, const char *func, PADDRINFOA addr_info)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    freeaddrinfo(addr_info);
}

void mock_freeaddrinfo(const char *file, const int line, const char *func, PADDRINFOA addr_info)
{
    if (_mock_winsock != nullptr)
    {
        _mock_winsock->freeaddrinfo(file, line, func, addr_info);
    }
    else
    {
        delegate_real_freeaddrinfo(file, line, func, addr_info);
    }

    TRACE_WINSOCK_CALL(freeaddrinfo);
}

#endif // _WIN32
