#include <test_com.h>
#include <sys/mock_stat.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_utimensat(const char *file, const int line, const char *func, int dirfd, const char *path,
                            const struct timespec *times, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return utimensat(dirfd, path, times, flags);
}

int mock_utimensat(const char *file, const int line, const char *func, int dirfd, const char *path,
                   const struct timespec *times, int flags)
{
    int mock_ret;

    if (_mock_sys_stat != nullptr)
    {
        mock_ret = _mock_sys_stat->utimensat(file, line, func, dirfd, path, times, flags);
    }
    else
    {
        mock_ret = delegate_real_utimensat(file, line, func, dirfd, path, times, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > utimensat %s", path);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
