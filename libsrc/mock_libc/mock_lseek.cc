#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

#ifndef _WIN32

off_t delegate_real_lseek(const char *file, const int line, const char *func, int fd, off_t offset, int whence)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return lseek(fd, offset, whence);
}

off_t mock_lseek(const char *file, const int line, const char *func, int fd, off_t offset, int whence)
{
    off_t rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->lseek(file, line, func, fd, offset, whence);
    }
    else
    {
        rtc = delegate_real_lseek(file, line, func, fd, offset, whence);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > lseek %d, %lld, %d", fd, (long long)offset, whence);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lld\n", file, line, (long long)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#else // _WIN32

__int64 delegate_real__lseeki64(const char *file, const int line, const char *func, int fd, __int64 offset, int whence)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _lseeki64(fd, offset, whence);
}

__int64 mock__lseeki64(const char *file, const int line, const char *func, int fd, __int64 offset, int whence)
{
    __int64 rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->_lseeki64(file, line, func, fd, offset, whence);
    }
    else
    {
        rtc = delegate_real__lseeki64(file, line, func, fd, offset, whence);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _lseeki64 %d, %lld, %d", fd, (long long)offset, whence);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lld\n", file, line, (long long)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
