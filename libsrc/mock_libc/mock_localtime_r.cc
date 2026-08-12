#ifndef _WIN32

    #include <test_com.h>
    #include <mock_time.h>

    #include <string.h>

using namespace testing;

struct tm *delegate_fake_localtime_r(const char *file, const int line, const char *func, const time_t *timep,
                                     struct tm *result)
{
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

struct tm *delegate_real_localtime_r(const char *file, const int line, const char *func, const time_t *timep,
                                     struct tm *result)
{
    (void)file;
    (void)line;
    (void)func;

    return localtime_r(timep, result);
}

struct tm *mock_localtime_r(const char *file, const int line, const char *func, const time_t *timep, struct tm *result)
{
    struct tm *ret;

    if (_mock_time != nullptr)
    {
        ret = _mock_time->localtime_r(file, line, func, timep, result);
    }
    else
    {
        ret = delegate_real_localtime_r(file, line, func, timep, result);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > localtime_r 0x%p, 0x%p", (const void *)timep, (void *)result);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)ret);
        }
        else
        {
            printf("\n");
        }
    }

    return ret;
}

#endif // _WIN32
