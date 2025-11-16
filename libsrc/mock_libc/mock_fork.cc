#ifndef _WIN32

#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

pid_t delegate_real_fork(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fork();
}

pid_t mock_fork(const char *file, const int line, const char *func)
{
    pid_t rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->fork(file, line, func);
    }
    else
    {
        rtc = delegate_real_fork(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fork");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
