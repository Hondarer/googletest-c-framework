#include <test_com.h>
#include <mock_termios.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_tcgetattr(const char *file, const int line, const char *func, int fd, struct termios *termios_p)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return tcgetattr(fd, termios_p);
}

int mock_tcgetattr(const char *file, const int line, const char *func, int fd, struct termios *termios_p)
{
    int mock_ret;

    if (_mock_termios != nullptr)
    {
        mock_ret = _mock_termios->tcgetattr(file, line, func, fd, termios_p);
    }
    else
    {
        mock_ret = delegate_real_tcgetattr(file, line, func, fd, termios_p);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > tcgetattr %d", fd);
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
