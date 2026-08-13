#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

int delegate_fake_rename(const char *file, const int line, const char *func, const char *oldpath, const char *newpath)
{
    (void)file;
    (void)line;
    (void)func;
    (void)oldpath;
    (void)newpath;

    return 0;
}

int delegate_real_rename(const char *file, const int line, const char *func, const char *oldpath, const char *newpath)
{
    (void)file;
    (void)line;
    (void)func;

    return ::rename(oldpath, newpath);
}

int mock_rename(const char *file, const int line, const char *func, const char *oldpath, const char *newpath)
{
    int rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->rename(file, line, func, oldpath, newpath);
    }
    else
    {
        rtc = delegate_real_rename(file, line, func, oldpath, newpath);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > rename %s, %s", oldpath, newpath);
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
