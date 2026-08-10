#include <test_com.h>
#include <sys/mock_socket.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_socket *_mock_sys_socket = nullptr;

Mock_sys_socket::Mock_sys_socket()
{
    ON_CALL(*this, socket(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_socket));
    ON_CALL(*this, sendto(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sendto));

    _mock_sys_socket = this;
}

Mock_sys_socket::~Mock_sys_socket()
{
    _mock_sys_socket = nullptr;
}

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

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > socket");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

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

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sendto");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
