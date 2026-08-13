#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_fsync(const char *file, const int line, const char *func, int fd)
{
    (void)file;
    (void)line;
    (void)func;

    return fsync(fd);
}

int mock_fsync(const char *file, const int line, const char *func, int fd)
{
    int result;

    if (_mock_unistd != nullptr)
    {
        result = _mock_unistd->fsync(file, line, func, fd);
    }
    else
    {
        result = delegate_real_fsync(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fsync");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
