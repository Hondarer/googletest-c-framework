#include <test_com.h>
#include <mock_stdlib.h>

using namespace testing;

void delegate_real_free(const char *file, const int line, const char *func, void *__ptr)
{
    // 未使用引数の警告を抑止します。
    (void)file;
    (void)line;
    (void)func;

    free(__ptr);
}

void mock_free(const char *file, const int line, const char *func, void *__ptr)
{
    if (_mock_stdlib != nullptr)
    {
        _mock_stdlib->free(file, line, func, __ptr);
    }
    else
    {
        delegate_real_free(file, line, func, __ptr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > free 0x%p", __ptr);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}
