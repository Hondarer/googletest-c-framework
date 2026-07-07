#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

#ifndef _WIN32

int delegate_real_dup2(const char *file, const int line, const char *func, int oldfd, int newfd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return dup2(oldfd, newfd);
}

int mock_dup2(const char *file, const int line, const char *func, int oldfd, int newfd)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->dup2(file, line, func, oldfd, newfd);
    }
    else
    {
        rtc = delegate_real_dup2(file, line, func, oldfd, newfd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > dup2 %d, %d", oldfd, newfd);
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

int delegate_real__dup2(const char *file, const int line, const char *func, int oldfd, int newfd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _dup2(oldfd, newfd);
}

int mock__dup2(const char *file, const int line, const char *func, int oldfd, int newfd)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->_dup2(file, line, func, oldfd, newfd);
    }
    else
    {
        rtc = delegate_real__dup2(file, line, func, oldfd, newfd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _dup2 %d, %d", oldfd, newfd);
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
