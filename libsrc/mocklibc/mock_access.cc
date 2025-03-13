#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

int delegate_real_access(const char *file, const int line, const char *func, const char *path, int amode)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return access(path, amode);
}

int mock_access(const char *file, const int line, const char *func, const char *path, int amode)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->access(file, line, func, path, amode);
    }
    else
    {
        rtc = delegate_real_access(file, line, func, path, amode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > access %s, %d", path, amode);
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
