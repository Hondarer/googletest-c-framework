#include <test_com.h>
#include <mock_stdlib.h>

using namespace testing;

void *delegate_real_malloc(const char *file, const int line, const char *func, size_t __size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return malloc(__size);
}

void *mock_malloc(const char *file, const int line, const char *func, size_t __size)
{
    void *mock_ret = NULL;

    if (_mock_stdlib != nullptr)
    {
        mock_ret = _mock_stdlib->malloc(file, line, func, __size);
    }
    else
    {
        mock_ret = delegate_real_malloc(file, line, func, __size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > malloc %zd", __size);
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
