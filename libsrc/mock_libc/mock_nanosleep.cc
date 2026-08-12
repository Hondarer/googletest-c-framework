#ifndef _WIN32

    #include <test_com.h>
    #include <mock_time.h>

    #include <string.h>

using namespace testing;

int delegate_fake_nanosleep(const char *file, const int line, const char *func, const struct timespec *req,
                            struct timespec *rem)
{
    (void)file;
    (void)line;
    (void)func;
    (void)req;

    if (rem != NULL)
    {
        memset(rem, 0, sizeof(*rem));
    }
    return 0;
}

int delegate_real_nanosleep(const char *file, const int line, const char *func, const struct timespec *req,
                            struct timespec *rem)
{
    (void)file;
    (void)line;
    (void)func;

    return nanosleep(req, rem);
}

int mock_nanosleep(const char *file, const int line, const char *func, const struct timespec *req, struct timespec *rem)
{
    int ret;

    if (_mock_time != nullptr)
    {
        ret = _mock_time->nanosleep(file, line, func, req, rem);
    }
    else
    {
        ret = delegate_real_nanosleep(file, line, func, req, rem);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > nanosleep 0x%p, 0x%p", (const void *)req, (void *)rem);
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
