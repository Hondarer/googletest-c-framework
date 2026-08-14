#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

int delegate_fake_fclose(const char *file, const int line, const char *func, FILE *fp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    free(fp);

    return 0;
}

int delegate_real_fclose(const char *file, const int line, const char *func, FILE *fp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return fclose(fp);
}

int mock_fclose(const char *file, const int line, const char *func, FILE *fp)
{
    int mock_ret;
    void *_fp = fp; // fclose 内にて初期化されるため、退避

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->fclose(file, line, func, fp);
    }
    else
    {
        mock_ret = delegate_real_fclose(file, line, func, fp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fclose 0x%p", (void *)_fp);
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
