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
    int mock_ret;

    if (_mock_fcntl != nullptr)
    {
        mock_ret = _mock_fcntl->open(file, line, func, path, flags, mode);
    }
    else
    {
        mock_ret = delegate_real_open(file, line, func, path, flags, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > open \"%s\", %d, %o", path, flags, mode);
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
