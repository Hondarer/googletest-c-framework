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
    int result;

    if (_mock_sys_file != nullptr)
    {
        result = _mock_sys_file->flock(file, line, func, fd, operation);
    }
    else
    {
        result = delegate_real_flock(file, line, func, fd, operation);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > flock %d, %d", fd, operation);
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
