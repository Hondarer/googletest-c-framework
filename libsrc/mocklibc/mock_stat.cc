#include <gmock/gmock.h>

#include <test_com.h>
#include <sys/mock_stat.h>

using namespace testing;

int delegate_real_stat(const char *file, const int line, const char *func, const char *path, struct stat *buf)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return stat(path, buf);
}

int mock_stat(const char *file, const int line, const char *func, const char *path, struct stat *buf)
{
    int rtc;

    if (_mock_sys_stat != nullptr)
    {
        rtc = _mock_sys_stat->stat(file, line, func, path, buf);
    }
    else
    {
        rtc = delegate_real_stat(file, line, func, path, buf);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > stat %s", path);
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
