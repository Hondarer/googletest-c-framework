#ifndef _WIN32

#include <test_com.h>
#include <mock_time.h>

#include <string.h>

using namespace testing;

char *delegate_fake_ctime_r(const char *file, const int line, const char *func, const time_t *timep, char *buf)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)timep;

    if (buf == NULL)
    {
        return NULL;
    }

    strcpy(buf, "Thu Jan  1 00:00:00 1970\n");

    return buf;
}

char *delegate_real_ctime_r(const char *file, const int line, const char *func, const time_t *timep, char *buf)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ctime_r(timep, buf);
}

char *mock_ctime_r(const char *file, const int line, const char *func, const time_t *timep, char *buf)
{
    char *rtc;

    if (_mock_time != nullptr)
    {
        rtc = _mock_time->ctime_r(file, line, func, timep, buf);
    }
    else
    {
        rtc = delegate_real_ctime_r(file, line, func, timep, buf);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ctime_r 0x%p, 0x%p", (const void *)timep, (void *)buf);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (rtc == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> %.24s\n", file, line, rtc);
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
