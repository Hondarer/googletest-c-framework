#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

int delegate_fake_feof(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;
    (void)stream;

    return 1;
}

int delegate_real_feof(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;

    return feof(stream);
}

int mock_feof(const char *file, const int line, const char *func, FILE *stream)
{
    int ret;

    if (_mock_stdio != nullptr)
    {
        ret = _mock_stdio->feof(file, line, func, stream);
    }
    else
    {
        ret = delegate_real_feof(file, line, func, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > feof 0x%p", (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, ret);
        }
        else
        {
            printf("\n");
        }
    }

    return ret;
}
