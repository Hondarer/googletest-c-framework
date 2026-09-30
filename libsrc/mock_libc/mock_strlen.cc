#include <test_com.h>
#include <mock_string.h>

using namespace testing;

size_t delegate_real_strlen(const char *file, const int line, const char *func, const char *s)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return strlen(s);
}

size_t mock_strlen(const char *file, const int line, const char *func, const char *s)
{
    size_t mock_ret;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->strlen(file, line, func, s);
    }
    else
    {
        mock_ret = delegate_real_strlen(file, line, func, s);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > strlen %s", s != nullptr ? s : "(null)");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %zu\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
