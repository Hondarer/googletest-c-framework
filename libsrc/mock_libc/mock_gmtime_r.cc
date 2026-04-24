#ifndef _WIN32

#include <test_com.h>
#include <mock_time.h>

#include <string.h>

using namespace testing;

struct tm *delegate_fake_gmtime_r(const char *file, const int line, const char *func, const time_t *timep, struct tm *result)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)timep;

    if (result == NULL)
    {
        return NULL;
    }

    memset(result, 0, sizeof(*result));

    return result;
}

struct tm *delegate_real_gmtime_r(const char *file, const int line, const char *func, const time_t *timep, struct tm *result)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return gmtime_r(timep, result);
}

struct tm *mock_gmtime_r(const char *file, const int line, const char *func, const time_t *timep, struct tm *result)
{
    struct tm *rtc;

    if (_mock_time != nullptr)
    {
        rtc = _mock_time->gmtime_r(file, line, func, timep, result);
    }
    else
    {
        rtc = delegate_real_gmtime_r(file, line, func, timep, result);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > gmtime_r 0x%p, 0x%p", (const void *)timep, (void *)result);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (rtc == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> %04d-%02d-%02d %02d:%02d:%02d\n",
                       file, line,
                       rtc->tm_year + 1900, rtc->tm_mon + 1, rtc->tm_mday,
                       rtc->tm_hour, rtc->tm_min, rtc->tm_sec);
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
