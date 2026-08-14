#include <test_com.h>
#include <mock_stdlib.h>

using namespace testing;

int delegate_real_atexit(const char *file, const int line, const char *func, mock_atexit_fn callback)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return atexit(callback);
}

int mock_atexit(const char *file, const int line, const char *func, mock_atexit_fn callback)
{
    int mock_ret;

    if (_mock_stdlib != nullptr)
    {
        mock_ret = _mock_stdlib->atexit(file, line, func, callback);
    }
    else
    {
        mock_ret = delegate_real_atexit(file, line, func, callback);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > atexit");
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
