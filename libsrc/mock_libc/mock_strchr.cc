#include <test_com.h>
#include <mock_string.h>

using namespace testing;

char *delegate_real_strchr(const char *file, const int line, const char *func, const char *s, int c)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return const_cast<char *>(strchr(s, c));
}

char *mock_strchr(const char *file, const int line, const char *func, const char *s, int c)
{
    char *mock_ret;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->strchr(file, line, func, s, c);
    }
    else
    {
        mock_ret = delegate_real_strchr(file, line, func, s, c);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > strchr %s, 0x%02x", s != nullptr ? s : "(null)", c);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (mock_ret == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
