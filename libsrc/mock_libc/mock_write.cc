#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

#ifndef _WIN32

ssize_t delegate_real_write(const char *file, const int line, const char *func, int fd, const void *buf, size_t count)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return write(fd, buf, count);
}

ssize_t mock_write(const char *file, const int line, const char *func, int fd, const void *buf, size_t count)
{
    ssize_t rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->write(file, line, func, fd, buf, count);
    }
    else
    {
        rtc = delegate_real_write(file, line, func, fd, buf, count);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > write %d, 0x%p, %zu", fd, buf, count);
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

int delegate_real__write(const char *file, const int line, const char *func, int fd, const void *buf,
                         unsigned int count)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _write(fd, buf, count);
}

int mock__write(const char *file, const int line, const char *func, int fd, const void *buf, unsigned int count)
{
    int rtc;

    if (_mock_unistd != nullptr)
    {
        rtc = _mock_unistd->_write(file, line, func, fd, buf, count);
    }
    else
    {
        rtc = delegate_real__write(file, line, func, fd, buf, count);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _write %d, 0x%p, %u", fd, buf, count);
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
