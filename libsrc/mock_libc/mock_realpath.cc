#include <test_com.h>
#include <mock_stdlib.h>

#ifndef _WIN32

using namespace testing;

char *delegate_real_realpath(const char *file, const int line, const char *func, const char *path, char *resolved)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return realpath(path, resolved);
}

char *mock_realpath(const char *file, const int line, const char *func, const char *path, char *resolved)
{
    char *result;

    if (_mock_stdlib != nullptr)
    {
        result = _mock_stdlib->realpath(file, line, func, path, resolved);
    }
    else
    {
        result = delegate_real_realpath(file, line, func, path, resolved);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > realpath");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
