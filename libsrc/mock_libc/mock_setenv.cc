#include <test_com.h>
#include <mock_stdlib.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_setenv(const char *file, const int line, const char *func, const char *name, const char *value,
                         int overwrite)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return setenv(name, value, overwrite);
}

int mock_setenv(const char *file, const int line, const char *func, const char *name, const char *value, int overwrite)
{
    int mock_ret;

    if (_mock_stdlib != nullptr)
    {
        mock_ret = _mock_stdlib->setenv(file, line, func, name, value, overwrite);
    }
    else
    {
        mock_ret = delegate_real_setenv(file, line, func, name, value, overwrite);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > setenv %s", name != nullptr ? name : "(null)");
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
