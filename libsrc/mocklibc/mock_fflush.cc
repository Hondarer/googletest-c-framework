#include <gmock/gmock.h>

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
    int rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->fflush(file, line, func, fp);
    }
    else
    {
        rtc = delegate_real_fflush(file, line, func, fp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fflush %d", fp->_fileno);
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
