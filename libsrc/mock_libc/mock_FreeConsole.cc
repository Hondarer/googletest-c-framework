#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_FreeConsole(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return FreeConsole();
}

BOOL mock_FreeConsole(const char *file, const int line, const char *func)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->FreeConsole(file, line, func);
    }
    else
    {
        mock_ret = delegate_real_FreeConsole(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > FreeConsole");
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
