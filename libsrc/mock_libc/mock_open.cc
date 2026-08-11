#include <test_com.h>
#include <mock_fcntl.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_open(const char *file, const int line, const char *func, const char *path, int flags, int mode)
{
    (void)file;
    (void)line;
    (void)func;

    return open(path, flags, (mode_t)mode);
}

int mock_open(const char *file, const int line, const char *func, const char *path, int flags, int mode)
{
    int result;

    if (_mock_fcntl != nullptr)
    {
        result = _mock_fcntl->open(file, line, func, path, flags, mode);
    }
    else
    {
        result = delegate_real_open(file, line, func, path, flags, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > open \"%s\", %d, %o", path, flags, mode);
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
