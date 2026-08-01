#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>
#include <string.h>

using namespace testing;

int delegate_real_vsnprintf(const char *file, const int line, const char *func, char *s, const size_t n,
                            const char *str)
{
    (void)file;
    (void)line;
    (void)func;

    return snprintf(s, n, "%s", str);
}

int delegate_fake_vsnprintf(const char *file, const int line, const char *func, char *s, const size_t n,
                            const char *str)
{
    (void)file;
    (void)line;
    (void)func;
    (void)s;
    (void)n;

    return (int)strlen(str);
}

int mock_vsnprintf(const char *file, const int line, const char *func, char *s, const size_t n, const char *format,
                   va_list ap)
{
    va_list args_copy;
    char *str;
    int rtc;

    va_copy(args_copy, ap);
    str = allocvprintf(format, args_copy);
    va_end(args_copy);

    if (str == NULL)
    {
        rtc = -1;
    }
    else if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->vsnprintf(file, line, func, s, n, str);
    }
    else
    {
        rtc = delegate_real_vsnprintf(file, line, func, s, n, str);
    }

    if (getTraceLevel() > TRACE_NONE && str != NULL)
    {
        printf("  > vsnprintf 0x%p, %zu, %s", (void *)s, n, str);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    free(str);

    return rtc;
}
