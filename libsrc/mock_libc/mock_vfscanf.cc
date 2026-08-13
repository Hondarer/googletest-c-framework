#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>

using namespace testing;

int delegate_real_vfscanf(const char *file, const int line, const char *func, FILE *stream, const char *format,
                          va_list arg_ptr)
{
    (void)file;
    (void)line;
    (void)func;

    return vfscanf(stream, format, arg_ptr);
}

int mock_vfscanf(const char *file, const int line, const char *func, FILE *stream, const char *format, va_list arg_ptr)
{
    int rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->vfscanf(file, line, func, stream, format, arg_ptr);
    }
    else
    {
        rtc = delegate_real_vfscanf(file, line, func, stream, format, arg_ptr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > vfscanf 0x%p, %s", (void *)stream, format);
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
