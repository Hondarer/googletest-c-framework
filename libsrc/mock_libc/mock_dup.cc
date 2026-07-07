#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

#ifndef _WIN32

int delegate_real_dup(const char *file, const int line, const char *func, int fd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return dup(fd);
}

int mock_dup(const char *file, const int line, const char *func, int fd)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->dup(file, line, func, fd);
    }
    else
    {
        rtc = delegate_real_dup(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > dup %d", fd);
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

#else // _WIN32

int delegate_real__dup(const char *file, const int line, const char *func, int fd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _dup(fd);
}

int mock__dup(const char *file, const int line, const char *func, int fd)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->_dup(file, line, func, fd);
    }
    else
    {
        rtc = delegate_real__dup(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _dup %d", fd);
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

#endif // _WIN32
