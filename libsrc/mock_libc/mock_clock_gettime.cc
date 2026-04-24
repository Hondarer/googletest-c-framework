#ifndef _WIN32

#include <test_com.h>
#include <mock_time.h>

using namespace testing;

int delegate_fake_clock_gettime(const char *file, const int line, const char *func, clockid_t clk_id, struct timespec *tp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)clk_id;

    if (tp == NULL)
    {
        return -1;
    }

    tp->tv_sec = 0;
    tp->tv_nsec = 0;

    return 0;
}

int delegate_real_clock_gettime(const char *file, const int line, const char *func, clockid_t clk_id, struct timespec *tp)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return clock_gettime(clk_id, tp);
}

int mock_clock_gettime(const char *file, const int line, const char *func, clockid_t clk_id, struct timespec *tp)
{
    int rtc;

    if (_mock_time != nullptr)
    {
        rtc = _mock_time->clock_gettime(file, line, func, clk_id, tp);
    }
    else
    {
        rtc = delegate_real_clock_gettime(file, line, func, clk_id, tp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > clock_gettime %d, 0x%p", (int)clk_id, (void *)tp);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (rtc != 0 || tp == NULL)
            {
                printf(" from %s:%d -> %d\n", file, line, rtc);
            }
            else
            {
                printf(" from %s:%d -> %lld, %ld\n",
                       file, line, (long long)tp->tv_sec, (long)tp->tv_nsec);
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
