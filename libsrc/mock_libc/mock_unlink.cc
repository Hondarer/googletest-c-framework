#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_unlink(const char *file, const int line, const char *func, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

    return ::unlink(path);
}

int mock_unlink(const char *file, const int line, const char *func, const char *path)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->unlink(file, line, func, path);
    }
    else
    {
        mock_ret = delegate_real_unlink(file, line, func, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > unlink %s", path);
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
