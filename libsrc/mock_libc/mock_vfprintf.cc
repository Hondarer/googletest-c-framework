#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>

using namespace testing;

int delegate_real_vfprintf(const char *file, const int line, const char *func, FILE *stream, const char *str)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fprintf(stream, "%s", str);
}

int delegate_fake_vfprintf(const char *file, const int line, const char *func, FILE *stream, const char *str)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)stream;

    return (int)strlen(str);
}

int mock_vfprintf(const char *file, const int line, const char *func, FILE *stream, const char *fmt, va_list ap)
{
    va_list args_copy;
    char *str;
    int mock_ret;

    va_copy(args_copy, ap);
    str = allocvprintf(fmt, args_copy);
    va_end(args_copy);

    if (str == NULL)
    {
        mock_ret = -1;
    }
    else if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->vfprintf(file, line, func, stream, str);
    }
    else
    {
        mock_ret = delegate_real_vfprintf(file, line, func, stream, str);
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
            printf("  > vfprintf 0x%p, %s", (void *)stream, trimmed_str);
            free(trimmed_str);
            if (getTraceLevel() >= TRACE_DETAIL)
            {
                printf(" from %s:%d -> %d\n", file, line, mock_ret);
            }
            else
            {
                printf("\n");
            }
        }
    }

    free(str);

    return mock_ret;
}
