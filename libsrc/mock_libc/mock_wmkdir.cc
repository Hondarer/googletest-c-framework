#ifdef _WIN32

    #include <mock_direct.h>
    #include <test_com.h>
    #include <../ucrt/direct.h>

using namespace testing;

int delegate_real_wmkdir(const char *file, const int line, const char *func, const wchar_t *path)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _wmkdir(path);
}

int mock_wmkdir(const char *file, const int line, const char *func, const wchar_t *path)
{
    int mock_ret;

    if (_mock_direct != nullptr)
    {
        mock_ret = _mock_direct->wmkdir(file, line, func, path);
    }
    else
    {
        mock_ret = delegate_real_wmkdir(file, line, func, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _wmkdir %ls", path);
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
