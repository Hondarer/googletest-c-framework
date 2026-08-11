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
    int rtc;

    if (_mock_stdlib != nullptr)
    {
        rtc = _mock_stdlib->atexit(file, line, func, callback);
    }
    else
    {
        rtc = delegate_real_atexit(file, line, func, callback);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > atexit");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
