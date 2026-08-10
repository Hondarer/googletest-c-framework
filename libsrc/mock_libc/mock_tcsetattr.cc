#include <test_com.h>
#include <mock_termios.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_tcsetattr(const char *file, const int line, const char *func, int fd, int optional_actions,
                            const struct termios *termios_p)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return tcsetattr(fd, optional_actions, termios_p);
}

int mock_tcsetattr(const char *file, const int line, const char *func, int fd, int optional_actions,
                   const struct termios *termios_p)
{
    int result;

    if (_mock_termios != nullptr)
    {
        result = _mock_termios->tcsetattr(file, line, func, fd, optional_actions, termios_p);
    }
    else
    {
        result = delegate_real_tcsetattr(file, line, func, fd, optional_actions, termios_p);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > tcsetattr %d", fd);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
