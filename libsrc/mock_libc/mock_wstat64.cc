#ifdef _WIN32

    #include <sys/mock_stat.h>
    #include <test_com.h>
    #include <../ucrt/sys/stat.h>

using namespace testing;

int delegate_real_wstat64(const char *file, const int line, const char *func, const wchar_t *path,
                          struct _stat64 *buf)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _wstat64(path, buf);
}

int mock_wstat64(const char *file, const int line, const char *func, const wchar_t *path, struct _stat64 *buf)
{
    int mock_ret;

    if (_mock_sys_stat != nullptr)
    {
        mock_ret = _mock_sys_stat->wstat64(file, line, func, path, buf);
    }
    else
    {
        mock_ret = delegate_real_wstat64(file, line, func, path, buf);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _wstat64 %ls, 0x%p", path, (void *)buf);
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
