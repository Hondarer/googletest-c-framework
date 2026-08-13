#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

int delegate_fake_remove(const char *file, const int line, const char *func, const char *path)
{
    (void)file;
    (void)line;
    (void)func;
    (void)path;

    return 0;
}

int delegate_real_remove(const char *file, const int line, const char *func, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

    return ::remove(path);
}

int mock_remove(const char *file, const int line, const char *func, const char *path)
{
    int rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->remove(file, line, func, path);
    }
    else
    {
        rtc = delegate_real_remove(file, line, func, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > remove %s", path);
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
