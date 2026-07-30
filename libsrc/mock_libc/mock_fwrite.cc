#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

size_t delegate_fake_fwrite(const char *file, const int line, const char *func, const void *ptr, size_t size,
                            size_t count, FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)ptr;
    (void)size;
    (void)stream;

    /* 書き込み系は実際には書き込まず、書き込んだ要素数相当を返す */
    return count;
}

size_t delegate_real_fwrite(const char *file, const int line, const char *func, const void *ptr, size_t size,
                            size_t count, FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fwrite(ptr, size, count, stream);
}

size_t mock_fwrite(const char *file, const int line, const char *func, const void *ptr, size_t size, size_t count,
                   FILE *stream)
{
    size_t rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->fwrite(file, line, func, ptr, size, count, stream);
    }
    else
    {
        rtc = delegate_real_fwrite(file, line, func, ptr, size, count, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fwrite 0x%p, %zu, %zu, 0x%p", ptr, size, count, (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %zu\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
