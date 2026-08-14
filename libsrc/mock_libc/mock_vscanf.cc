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
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->vscanf(file, line, func, format, arg_ptr);
    }
    else
    {
        mock_ret = delegate_real_vscanf(file, line, func, format, arg_ptr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > vscanf %s", format);
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
