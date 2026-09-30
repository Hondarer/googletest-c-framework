#include <test_com.h>
#include <mock_string.h>

#include <limits.h>

using namespace testing;

int delegate_real_strncmp(const char *file, const int line, const char *func, const char *s1, const char *s2, size_t n)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return strncmp(s1, s2, n);
}

int mock_strncmp(const char *file, const int line, const char *func, const char *s1, const char *s2, size_t n)
{
    int mock_ret;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->strncmp(file, line, func, s1, s2, n);
    }
    else
    {
        mock_ret = delegate_real_strncmp(file, line, func, s1, s2, n);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > strncmp %.*s, %.*s, %zu", (int)(n < (size_t)INT_MAX ? n : (size_t)INT_MAX),
               s1 != nullptr ? s1 : "(null)", (int)(n < (size_t)INT_MAX ? n : (size_t)INT_MAX),
               s2 != nullptr ? s2 : "(null)", n);
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
