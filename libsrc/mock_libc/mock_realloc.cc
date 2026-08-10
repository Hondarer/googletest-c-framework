#include <test_com.h>
#include <mock_stdlib.h>

using namespace testing;

void *delegate_real_realloc(const char *file, const int line, const char *func, void *__ptr, size_t __size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return realloc(__ptr, __size);
}

void *mock_realloc(const char *file, const int line, const char *func, void *__ptr, size_t __size)
{
    void *result = NULL;

    if (_mock_stdlib != nullptr)
    {
        result = _mock_stdlib->realloc(file, line, func, __ptr, __size);
    }
    else
    {
        result = delegate_real_realloc(file, line, func, __ptr, __size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > realloc 0x%p, %zd", __ptr, __size);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (result == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> 0x%p\n", file, line, result);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}
