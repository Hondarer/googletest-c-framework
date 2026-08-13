#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>

using namespace testing;

int delegate_real_vscanf(const char *file, const int line, const char *func, const char *format, va_list arg_ptr)
{
    (void)file;
    (void)line;
    (void)func;

    return vscanf(format, arg_ptr);
}

int mock_vscanf(const char *file, const int line, const char *func, const char *format, va_list arg_ptr)
{
    int rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->vscanf(file, line, func, format, arg_ptr);
    }
    else
    {
        rtc = delegate_real_vscanf(file, line, func, format, arg_ptr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > vscanf %s", format);
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
