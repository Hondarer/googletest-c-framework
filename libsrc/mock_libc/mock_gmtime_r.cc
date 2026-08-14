#ifndef _WIN32

    #include <test_com.h>
    #include <mock_time.h>

    #include <string.h>

using namespace testing;

struct tm *delegate_fake_gmtime_r(const char *file, const int line, const char *func, const time_t *timep,
                                  struct tm *result)
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

struct tm *delegate_real_gmtime_r(const char *file, const int line, const char *func, const time_t *timep,
                                  struct tm *result)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return gmtime_r(timep, result);
}

struct tm *mock_gmtime_r(const char *file, const int line, const char *func, const time_t *timep, struct tm *result)
{
    struct tm *mock_ret;

    if (_mock_time != nullptr)
    {
        mock_ret = _mock_time->gmtime_r(file, line, func, timep, result);
    }
    else
    {
        mock_ret = delegate_real_gmtime_r(file, line, func, timep, result);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > gmtime_r 0x%p, 0x%p", (const void *)timep, (void *)result);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (mock_ret == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> %04d-%02d-%02d %02d:%02d:%02d\n", file, line, mock_ret->tm_year + 1900,
                       mock_ret->tm_mon + 1, mock_ret->tm_mday, mock_ret->tm_hour, mock_ret->tm_min, mock_ret->tm_sec);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
