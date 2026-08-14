#include <test_com.h>
#include <sys/mock_file.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_flock(const char *file, const int line, const char *func, int fd, int operation)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return flock(fd, operation);
}

int mock_flock(const char *file, const int line, const char *func, int fd, int operation)
{
    int mock_ret;

    if (_mock_sys_file != nullptr)
    {
        mock_ret = _mock_sys_file->flock(file, line, func, fd, operation);
    }
    else
    {
        mock_ret = delegate_real_flock(file, line, func, fd, operation);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > flock %d, %d", fd, operation);
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
