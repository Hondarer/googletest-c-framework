#include <test_com.h>
#include <mock_unistd.h>

using namespace testing;

#ifndef _WIN32

int delegate_real_close(const char *file, const int line, const char *func, int fd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return close(fd);
}

int mock_close(const char *file, const int line, const char *func, int fd)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->close(file, line, func, fd);
    }
    else
    {
        mock_ret = delegate_real_close(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > close %d", fd);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#else // _WIN32

int delegate_real__close(const char *file, const int line, const char *func, int fd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _close(fd);
}

int mock__close(const char *file, const int line, const char *func, int fd)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->_close(file, line, func, fd);
    }
    else
    {
        mock_ret = delegate_real__close(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _close %d", fd);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
