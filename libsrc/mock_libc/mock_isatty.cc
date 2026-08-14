#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_isatty(const char *file, const int line, const char *func, int fd)
{
    (void)file;
    (void)line;
    (void)func;

    return isatty(fd);
}

int mock_isatty(const char *file, const int line, const char *func, int fd)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->isatty(file, line, func, fd);
    }
    else
    {
        mock_ret = delegate_real_isatty(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > isatty %d", fd);
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

#endif /* !_WIN32 */
