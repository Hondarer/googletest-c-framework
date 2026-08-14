#include <test_com.h>
#include <mock_string.h>

#ifndef _WIN32

using namespace testing;

char *delegate_real_strdup(const char *file, const int line, const char *func, const char *s)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return strdup(s);
}

char *mock_strdup(const char *file, const int line, const char *func, const char *s)
{
    char *mock_ret = NULL;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->strdup(file, line, func, s);
    }
    else
    {
        mock_ret = delegate_real_strdup(file, line, func, s);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > strdup %s", s != nullptr ? s : "(null)");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %s\n", file, line, mock_ret != nullptr ? "0x(dup)" : "NULL");
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
