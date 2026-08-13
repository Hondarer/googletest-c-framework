#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_unlink(const char *file, const int line, const char *func, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

    return ::unlink(path);
}

int mock_unlink(const char *file, const int line, const char *func, const char *path)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->unlink(file, line, func, path);
    }
    else
    {
        rtc = delegate_real_unlink(file, line, func, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > unlink %s", path);
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

#endif /* !_WIN32 */
