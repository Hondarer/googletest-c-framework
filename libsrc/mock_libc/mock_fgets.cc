#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

char *delegate_fake_fgets(const char *file, const int line, const char *func, char *s, int n, FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)s;
    (void)n;
    (void)stream;

    return NULL;
}

char *delegate_real_fgets(const char *file, const int line, const char *func, char *s, int n, FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fgets(s, n, stream);
}

char *mock_fgets(const char *file, const int line, const char *func, char *s, int n, FILE *stream)
{
    char *mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->fgets(file, line, func, s, n, stream);
    }
    else
    {
        mock_ret = delegate_real_fgets(file, line, func, s, n, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fgets 0x%p, %d, 0x%p", s, n, (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %s\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
