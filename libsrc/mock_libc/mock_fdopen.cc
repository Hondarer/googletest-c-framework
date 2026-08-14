#ifndef _WIN32

    #include <test_com.h>
    #include <mock_stdio.h>

    #include <stdlib.h>

using namespace testing;

FILE *delegate_fake_fdopen(const char *file, const int line, const char *func, int fd, const char *modes)
{
    (void)file;
    (void)line;
    (void)func;
    (void)fd;
    (void)modes;

    return (FILE *)malloc(sizeof(FILE));
}

FILE *delegate_real_fdopen(const char *file, const int line, const char *func, int fd, const char *modes)
{
    (void)file;
    (void)line;
    (void)func;

    return fdopen(fd, modes);
}

FILE *mock_fdopen(const char *file, const int line, const char *func, int fd, const char *modes)
{
    FILE *mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->fdopen(file, line, func, fd, modes);
    }
    else
    {
        mock_ret = delegate_real_fdopen(file, line, func, fd, modes);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fdopen %d, %s", fd, modes);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif /* !_WIN32 */
