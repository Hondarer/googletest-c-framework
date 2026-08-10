#include <test_com.h>
#include <mock_stdio.h>

#include <stdarg.h>
#include <string.h>

using namespace testing;

/* 可変長引数は展開済みの文字列として Mock クラスへ渡す。
 * see: .claude/skills/create-testfw-mock/SKILL.md の「printf 系モック」 */
int delegate_real_snprintf(const char *file, const int line, const char *func, char *s, const size_t n,
                           const char *str)
{
    (void)file;
    (void)line;
    (void)func;

    return snprintf(s, n, "%s", str);
}

int mock_snprintf(const char *file, const int line, const char *func, char *s, const size_t n, const char *format,
                  ...)
{
    va_list args;
    char *str;
    int rtc;

    va_start(args, format);
    str = allocvprintf(format, args);
    va_end(args);

    if (str == NULL)
    {
        rtc = -1;
    }
    else if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->snprintf(file, line, func, s, n, str);
    }
    else
    {
        rtc = delegate_real_snprintf(file, line, func, s, n, str);
    }

    if (getTraceLevel() > TRACE_NONE && str != NULL)
    {
        printf("  > snprintf 0x%p, %zu, %s", (void *)s, n, str);
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
