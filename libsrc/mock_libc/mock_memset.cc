#include <test_com.h>
#include <mock_string.h>

using namespace testing;

void *delegate_real_memset(const char *file, const int line, const char *func, void *s, int c, size_t n)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return memset(s, c, n);
}

void *mock_memset(const char *file, const int line, const char *func, void *s, int c, size_t n)
{
    void *mock_ret = NULL;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->memset(file, line, func, s, c, n);
    }
    else
    {
        mock_ret = delegate_real_memset(file, line, func, s, c, n);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > memset 0x%p, 0x%02x, %zd", s, c, n);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (mock_ret == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> 0x%p\n", file, line, mock_ret);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
