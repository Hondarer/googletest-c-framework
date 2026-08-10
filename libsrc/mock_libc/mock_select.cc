#include <test_com.h>
#include <sys/mock_select.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_select(const char *file, const int line, const char *func, int nfds, fd_set *readfds,
                         fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return select(nfds, readfds, writefds, exceptfds, timeout);
}

int mock_select(const char *file, const int line, const char *func, int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout)
{
    int result;

    if (_mock_sys_select != nullptr)
    {
        result = _mock_sys_select->select(file, line, func, nfds, readfds, writefds, exceptfds, timeout);
    }
    else
    {
        result = delegate_real_select(file, line, func, nfds, readfds, writefds, exceptfds, timeout);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > select %d", nfds);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
