#include <mock_instance.h>
#include <test_com.h>
#include <mock_netdb.h>

#ifndef _WIN32

using namespace testing;

Mock_netdb *_mock_netdb = nullptr;

Mock_netdb::Mock_netdb()
{
    ON_CALL(*this, getaddrinfo(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_getaddrinfo));
    ON_CALL(*this, freeaddrinfo(_, _, _, _)).WillByDefault(Invoke(delegate_real_freeaddrinfo));
    ON_CALL(*this, gai_strerror(_, _, _, _)).WillByDefault(Invoke(delegate_real_gai_strerror));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_netdb);
}

Mock_netdb::~Mock_netdb()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_netdb);
}

int delegate_real_getaddrinfo(const char *file, const int line, const char *func, const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getaddrinfo(node, service, hints, res);
}

int mock_getaddrinfo(const char *file, const int line, const char *func, const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    int mock_ret;

    if (_mock_netdb != nullptr)
    {
        mock_ret = _mock_netdb->getaddrinfo(file, line, func, node, service, hints, res);
    }
    else
    {
        mock_ret = delegate_real_getaddrinfo(file, line, func, node, service, hints, res);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > getaddrinfo");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

void delegate_real_freeaddrinfo(const char *file, const int line, const char *func, struct addrinfo *res)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    freeaddrinfo(res);
}

void mock_freeaddrinfo(const char *file, const int line, const char *func, struct addrinfo *res)
{
    if (_mock_netdb != nullptr)
    {
        _mock_netdb->freeaddrinfo(file, line, func, res);
    }
    else
    {
        delegate_real_freeaddrinfo(file, line, func, res);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > freeaddrinfo");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}

const char *delegate_real_gai_strerror(const char *file, const int line, const char *func, int errcode)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return gai_strerror(errcode);
}

const char *mock_gai_strerror(const char *file, const int line, const char *func, int errcode)
{
    const char *mock_ret;

    if (_mock_netdb != nullptr)
    {
        mock_ret = _mock_netdb->gai_strerror(file, line, func, errcode);
    }
    else
    {
        mock_ret = delegate_real_gai_strerror(file, line, func, errcode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > gai_strerror");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
