#include <test_com.h>
#include <mock_stdlib.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_unsetenv(const char *file, const int line, const char *func, const char *name)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return unsetenv(name);
}

int mock_unsetenv(const char *file, const int line, const char *func, const char *name)
{
    int mock_ret;

    if (_mock_stdlib != nullptr)
    {
        mock_ret = _mock_stdlib->unsetenv(file, line, func, name);
    }
    else
    {
        mock_ret = delegate_real_unsetenv(file, line, func, name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > unsetenv %s", name != nullptr ? name : "(null)");
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
