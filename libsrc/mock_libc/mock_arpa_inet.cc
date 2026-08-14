#include <test_com.h>
#include <arpa/mock_inet.h>

#ifndef _WIN32

using namespace testing;

Mock_arpa_inet *_mock_arpa_inet = nullptr;

Mock_arpa_inet::Mock_arpa_inet()
{
    ON_CALL(*this, inet_pton(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_inet_pton));
    ON_CALL(*this, inet_ntop(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_inet_ntop));

    _mock_arpa_inet = this;
}

Mock_arpa_inet::~Mock_arpa_inet()
{
    _mock_arpa_inet = nullptr;
}

int delegate_real_inet_pton(const char *file, const int line, const char *func, int af, const char *src, void *dst)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return inet_pton(af, src, dst);
}

int mock_inet_pton(const char *file, const int line, const char *func, int af, const char *src, void *dst)
{
    int mock_ret;

    if (_mock_arpa_inet != nullptr)
    {
        mock_ret = _mock_arpa_inet->inet_pton(file, line, func, af, src, dst);
    }
    else
    {
        mock_ret = delegate_real_inet_pton(file, line, func, af, src, dst);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > inet_pton");
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

const char *delegate_real_inet_ntop(const char *file, const int line, const char *func, int af, const void *src, char *dst, socklen_t size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return inet_ntop(af, src, dst, size);
}

const char *mock_inet_ntop(const char *file, const int line, const char *func, int af, const void *src, char *dst, socklen_t size)
{
    const char *mock_ret;

    if (_mock_arpa_inet != nullptr)
    {
        mock_ret = _mock_arpa_inet->inet_ntop(file, line, func, af, src, dst, size);
    }
    else
    {
        mock_ret = delegate_real_inet_ntop(file, line, func, af, src, dst, size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > inet_ntop");
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
