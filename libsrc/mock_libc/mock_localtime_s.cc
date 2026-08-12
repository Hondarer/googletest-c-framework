#ifdef _WIN32

    #include <test_com.h>
    #include <mock_time.h>

    #include <string.h>

using namespace testing;

errno_t delegate_fake_localtime_s(const char *file, const int line, const char *func, struct tm *local_tm,
                                  const time_t *timep)
{
    (void)file;
    (void)line;
    (void)func;
    (void)timep;

    if (local_tm == NULL)
    {
        return EINVAL;
    }

    memset(local_tm, 0, sizeof(*local_tm));
    return 0;
}

errno_t delegate_real_localtime_s(const char *file, const int line, const char *func, struct tm *local_tm,
                                  const time_t *timep)
{
    (void)file;
    (void)line;
    (void)func;

    return localtime_s(local_tm, timep);
}

errno_t mock_localtime_s(const char *file, const int line, const char *func, struct tm *local_tm, const time_t *timep)
{
    errno_t ret;

    if (_mock_time != nullptr)
    {
        ret = _mock_time->localtime_s(file, line, func, local_tm, timep);
    }
    else
    {
        ret = delegate_real_localtime_s(file, line, func, local_tm, timep);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > localtime_s 0x%p, 0x%p", (void *)local_tm, (const void *)timep);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, ret);
        }
        else
        {
            printf("\n");
        }
    }

    return ret;
}

#endif // _WIN32
