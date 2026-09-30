#include <test_com.h>
#include <mock_string.h>

using namespace testing;

int delegate_real_strcmp(const char *file, const int line, const char *func, const char *s1, const char *s2)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return strcmp(s1, s2);
}

int mock_strcmp(const char *file, const int line, const char *func, const char *s1, const char *s2)
{
    int mock_ret;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->strcmp(file, line, func, s1, s2);
    }
    else
    {
        mock_ret = delegate_real_strcmp(file, line, func, s1, s2);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > strcmp %s, %s", s1 != nullptr ? s1 : "(null)", s2 != nullptr ? s2 : "(null)");
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
