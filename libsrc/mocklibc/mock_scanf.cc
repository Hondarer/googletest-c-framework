#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>

using namespace testing;

int delegate_real_scanf(const char *file, const int line, const char *func, const char *format, va_list arg_ptr)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return vscanf(format, arg_ptr);
}

int mock_scanf(const char *file, const int line, const char *func, const char *fmt, ...)
{
    va_list args;
    int rtc;

    // 可変引数リストを初期化
    va_start(args, fmt);

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->scanf(file, line, func, fmt, args);
    }
    else
    {
        rtc = delegate_real_scanf(file, line, func, fmt, args);
    }

    va_end(args);

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > scanf %s", fmt);
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
