#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_rmdir(const char *file, const int line, const char *func, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

    return rmdir(path);
}

int mock_rmdir(const char *file, const int line, const char *func, const char *path)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->rmdir(file, line, func, path);
    }
    else
    {
        mock_ret = delegate_real_rmdir(file, line, func, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > rmdir %s", path);
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
