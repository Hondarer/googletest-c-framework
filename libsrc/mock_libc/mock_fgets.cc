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
    char *rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->fgets(file, line, func, s, n, stream);
    }
    else
    {
        rtc = delegate_real_fgets(file, line, func, s, n, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fgets 0x%p, %d, %p", s, n, (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %s\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
