#include <test_com.h>
#include <sys/mock_stat.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_fstat(const char *file, const int line, const char *func, int fd, struct stat *buf)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fstat(fd, buf);
}

int mock_fstat(const char *file, const int line, const char *func, int fd, struct stat *buf)
{
    int mock_ret;

    if (_mock_sys_stat != nullptr)
    {
        mock_ret = _mock_sys_stat->fstat(file, line, func, fd, buf);
    }
    else
    {
        mock_ret = delegate_real_fstat(file, line, func, fd, buf);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fstat %d", fd);
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
