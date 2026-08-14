#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

size_t delegate_fake_fread(const char *file, const int line, const char *func, void *ptr, size_t size, size_t count,
                           FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)ptr;
    (void)size;
    (void)count;
    (void)stream;

    /* 読み込み系は空状態 (要素数 0) を返す */
    return 0U;
}

size_t delegate_real_fread(const char *file, const int line, const char *func, void *ptr, size_t size, size_t count,
                           FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fread(ptr, size, count, stream);
}

size_t mock_fread(const char *file, const int line, const char *func, void *ptr, size_t size, size_t count,
                  FILE *stream)
{
    size_t mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->fread(file, line, func, ptr, size, count, stream);
    }
    else
    {
        mock_ret = delegate_real_fread(file, line, func, ptr, size, count, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fread 0x%p, %zu, %zu, 0x%p", ptr, size, count, (void *)stream);
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
