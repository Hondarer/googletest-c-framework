#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

int delegate_fake_fflush(const char *file, const int line, const char *func, FILE *fp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)fp;

    return 0;
}

int delegate_real_fflush(const char *file, const int line, const char *func, FILE *fp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fflush(fp);
}

int mock_fflush(const char *file, const int line, const char *func, FILE *fp)
{
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->fflush(file, line, func, fp);
    }
    else
    {
        mock_ret = delegate_real_fflush(file, line, func, fp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fflush 0x%p", (void *)fp);
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
