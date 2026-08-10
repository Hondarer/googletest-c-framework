#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_ftruncate(const char *file, const int line, const char *func, int fd, off_t length)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ftruncate(fd, length);
}

int mock_ftruncate(const char *file, const int line, const char *func, int fd, off_t length)
{
    int result;

    if (_mock_unistd != nullptr)
    {
        result = _mock_unistd->ftruncate(file, line, func, fd, length);
    }
    else
    {
        result = delegate_real_ftruncate(file, line, func, fd, length);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ftruncate");
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
