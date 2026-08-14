#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

int delegate_fake_ferror(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;
    (void)stream;

    return 0;
}

int delegate_real_ferror(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;

    return ferror(stream);
}

int mock_ferror(const char *file, const int line, const char *func, FILE *stream)
{
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->ferror(file, line, func, stream);
    }
    else
    {
        mock_ret = delegate_real_ferror(file, line, func, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ferror 0x%p", (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
