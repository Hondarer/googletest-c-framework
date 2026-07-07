#ifdef _WIN32

#include <test_com.h>
#include <mock_time.h>

#include <string.h>
#include <errno.h>

using namespace testing;

errno_t delegate_fake_ctime_s(const char *file, const int line, const char *func, char *buf, size_t size, const time_t *timep)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)timep;

    if (buf == NULL || size < 26)
    {
        return EINVAL;
    }

    strcpy(buf, "Thu Jan  1 00:00:00 1970\n");

    return 0;
}

errno_t delegate_real_ctime_s(const char *file, const int line, const char *func, char *buf, size_t size, const time_t *timep)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ctime_s(buf, size, timep);
}

errno_t mock_ctime_s(const char *file, const int line, const char *func, char *buf, size_t size, const time_t *timep)
{
    errno_t rtc;

    if (_mock_time != nullptr)
    {
        rtc = _mock_time->ctime_s(file, line, func, buf, size, timep);
    }
    else
    {
        rtc = delegate_real_ctime_s(file, line, func, buf, size, timep);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ctime_s 0x%p, %zu, 0x%p", (void *)buf, size, (const void *)timep);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (rtc != 0 || buf == NULL)
            {
                printf(" from %s:%d -> %d\n", file, line, (int)rtc);
            }
            else
            {
                printf(" from %s:%d -> %.24s\n", file, line, buf);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
