#ifndef _WIN32

    #include <test_com.h>
    #include <sys/mock_stat.h>

using namespace testing;

int delegate_real_mkdir(const char *file, const int line, const char *func, const char *path, mode_t mode)
{
    (void)file;
    (void)line;
    (void)func;

    return mkdir(path, mode);
}

int mock_mkdir(const char *file, const int line, const char *func, const char *path, mode_t mode)
{
    int mock_ret;

    if (_mock_sys_stat != nullptr)
    {
        mock_ret = _mock_sys_stat->mkdir(file, line, func, path, mode);
    }
    else
    {
        mock_ret = delegate_real_mkdir(file, line, func, path, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > mkdir %s, %o", path, mode);
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
