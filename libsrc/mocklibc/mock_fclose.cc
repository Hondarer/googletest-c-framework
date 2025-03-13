#include <gmock/gmock.h>

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
    int rtc;
    int fileno = fp->_fileno; // fclose 内にて初期化されるため、退避

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->fclose(file, line, func, fp);
    }
    else
    {
        rtc = delegate_real_fclose(file, line, func, fp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fclose %d", fileno);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
