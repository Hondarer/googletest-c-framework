#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetConsoleMode(const char *file, const int line, const char *func, HANDLE console_handle,
                                  DWORD mode)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return SetConsoleMode(console_handle, mode);
}

BOOL mock_SetConsoleMode(const char *file, const int line, const char *func, HANDLE console_handle, DWORD mode)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetConsoleMode(file, line, func, console_handle, mode);
    }
    else
    {
        mock_ret = delegate_real_SetConsoleMode(file, line, func, console_handle, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetConsoleMode 0x%p, %lu", (void *)console_handle, mode);
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
