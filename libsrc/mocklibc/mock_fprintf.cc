#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>

using namespace testing;

int delegate_real_fprintf(const char *file, const int line, const char *func, FILE *stream, const char *str)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fprintf(stream, "%s", str);
}

int delegate_fake_fprintf(const char *file, const int line, const char *func, FILE *stream, const char *str)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)stream;

    return strlen(str);
}

int mock_fprintf(const char *file, const int line, const char *func, FILE *stream, const char *fmt, ...)
{
    va_list args;
    char *str;
    int rtc;

    // 可変引数リストを初期化
    va_start(args, fmt);

    str = allocvprintf(fmt, args);
    va_end(args);

    if (str == NULL)
    {
        rtc = -1;
    }
    else if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->fprintf(file, line, func, stream, str);
    }
    else
    {
        rtc = delegate_real_fprintf(file, line, func, stream, str);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        size_t len = strlen(str);
        char *trimmed_str = (char *)malloc(len + 1);
        memcpy(trimmed_str, str, len + 1);
        if (trimmed_str != NULL)
        {
            if (len > 0 && trimmed_str[len - 1] == '\n')
            {
                trimmed_str[len - 1] = '\0';
            }
            printf("  > fprintf %d, %s", stream->_fileno, trimmed_str);
            free(trimmed_str);
            if (getTraceLevel() >= TRACE_DETAIL)
            {
                printf(" from %s:%d -> %d\n", file, line, rtc);
            }
            else
            {
                printf("\n");
            }
        }
    }

    free(str);

    return rtc;
}
