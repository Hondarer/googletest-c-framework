#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_stdlib.h>

using namespace testing;

void *delegate_real_calloc(const char *file, const int line, const char *func, size_t __nmemb, size_t __size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return calloc(__nmemb, __size);
}

void *mock_calloc(const char *file, const int line, const char *func, size_t __nmemb, size_t __size)
{
    void *result = NULL;

    if (_mock_stdlib != nullptr)
    {
        result = _mock_stdlib->calloc(file, line, func, __nmemb, __size);
    }
    else
    {
        result = delegate_real_calloc(file, line, func, __nmemb, __size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > calloc %ld, %ld", __nmemb, __size);
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
