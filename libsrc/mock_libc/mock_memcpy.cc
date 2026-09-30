#include <test_com.h>
#include <mock_string.h>

using namespace testing;

void *delegate_real_memcpy(const char *file, const int line, const char *func, void *dest, const void *src, size_t n)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return memcpy(dest, src, n);
}

void *mock_memcpy(const char *file, const int line, const char *func, void *dest, const void *src, size_t n)
{
    void *mock_ret;

    if (_mock_string != nullptr)
    {
        mock_ret = _mock_string->memcpy(file, line, func, dest, src, n);
    }
    else
    {
        mock_ret = delegate_real_memcpy(file, line, func, dest, src, n);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > memcpy 0x%p, 0x%p, %zu", dest, src, n);
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
