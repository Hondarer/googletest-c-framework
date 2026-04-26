#include <test_com.h>
#include <mock_stdlib.h>

using namespace testing;

char *delegate_real_getenv(const char *file, const int line, const char *func, const char *name)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getenv(name);
}

char *mock_getenv(const char *file, const int line, const char *func, const char *name)
{
    char *rtc;

    if (_mock_stdlib != nullptr)
    {
        rtc = _mock_stdlib->getenv(file, line, func, name);
    }
    else
    {
        rtc = delegate_real_getenv(file, line, func, name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > getenv %s", name);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %s\n", file, line, rtc != nullptr ? rtc : "(null)");
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
