#include <test_com.h>
#include <mock_poll.h>

#ifndef _WIN32

using namespace testing;

Mock_poll *_mock_poll = nullptr;

Mock_poll::Mock_poll()
{
    ON_CALL(*this, poll(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_poll));

    _mock_poll = this;
}

Mock_poll::~Mock_poll()
{
    _mock_poll = nullptr;
}

int delegate_real_poll(const char *file, const int line, const char *func, struct pollfd *fds, nfds_t nfds, int timeout)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return poll(fds, nfds, timeout);
}

int mock_poll(const char *file, const int line, const char *func, struct pollfd *fds, nfds_t nfds, int timeout)
{
    int mock_ret;

    if (_mock_poll != nullptr)
    {
        mock_ret = _mock_poll->poll(file, line, func, fds, nfds, timeout);
    }
    else
    {
        mock_ret = delegate_real_poll(file, line, func, fds, nfds, timeout);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > poll");
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
