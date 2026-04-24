#ifdef _WIN32

#include <test_com.h>
#include <mock_time.h>

#include <string.h>
#include <errno.h>

using namespace testing;

errno_t delegate_fake_gmtime_s(const char *file, const int line, const char *func, struct tm *utc_tm, const time_t *timep)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)timep;

    if (utc_tm == NULL)
    {
        return EINVAL;
    }

    memset(utc_tm, 0, sizeof(*utc_tm));

    return 0;
}

errno_t delegate_real_gmtime_s(const char *file, const int line, const char *func, struct tm *utc_tm, const time_t *timep)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return gmtime_s(utc_tm, timep);
}

errno_t mock_gmtime_s(const char *file, const int line, const char *func, struct tm *utc_tm, const time_t *timep)
{
    errno_t rtc;

    if (_mock_time != nullptr)
    {
        rtc = _mock_time->gmtime_s(file, line, func, utc_tm, timep);
    }
    else
    {
        rtc = delegate_real_gmtime_s(file, line, func, utc_tm, timep);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > gmtime_s 0x%p, 0x%p", (void *)utc_tm, (const void *)timep);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (rtc != 0 || utc_tm == NULL)
            {
                printf(" from %s:%d -> %d\n", file, line, (int)rtc);
            }
            else
            {
                printf(" from %s:%d -> %04d-%02d-%02d %02d:%02d:%02d\n",
                       file, line,
                       utc_tm->tm_year + 1900, utc_tm->tm_mon + 1, utc_tm->tm_mday,
                       utc_tm->tm_hour, utc_tm->tm_min, utc_tm->tm_sec);
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
