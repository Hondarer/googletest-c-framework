#include <test_com.h>
#include <sys/mock_wait.h>

using namespace testing;

pid_t delegate_real_waitpid(const char *file, const int line, const char *func, pid_t pid, int *stat_loc, int options)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return waitpid(pid, stat_loc, options);
}

pid_t mock_waitpid(const char *file, const int line, const char *func, pid_t pid, int *stat_loc, int options)
{
    pid_t rtc;

    if (_mock_sys_wait != nullptr)
    {
        rtc = _mock_sys_wait->waitpid(file, line, func, pid, stat_loc, options);
    }
    else
    {
        rtc = delegate_real_waitpid(file, line, func, pid, stat_loc, options);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        if (stat_loc == NULL)
        {
            printf("  > waitpid %d, NULL, %d", pid, options);
        }
        else
        {
            printf("  > waitpid %d, %d, %d", pid, *stat_loc, options);
        }

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
