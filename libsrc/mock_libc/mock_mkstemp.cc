#ifndef _WIN32

#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

int delegate_real_mkstemp(const char *file, const int line, const char *func, char *tmpl)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return mkstemp(tmpl);
}

int mock_mkstemp(const char *file, const int line, const char *func, char *tmpl)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->mkstemp(file, line, func, tmpl);
    }
    else
    {
        rtc = delegate_real_mkstemp(file, line, func, tmpl);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > mkstemp %s", tmpl);
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
