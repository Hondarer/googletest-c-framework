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
    int rtc;

    if (_mock_stdlib != nullptr)
    {
        rtc = _mock_stdlib->setenv(file, line, func, name, value, overwrite);
    }
    else
    {
        rtc = delegate_real_setenv(file, line, func, name, value, overwrite);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > setenv %s", name != nullptr ? name : "(null)");
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

#endif // _WIN32
