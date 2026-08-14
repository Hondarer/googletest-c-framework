#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetConsoleScreenBufferInfo(const char *file, const int line, const char *func, HANDLE console_handle,
                                              PCONSOLE_SCREEN_BUFFER_INFO info)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetConsoleScreenBufferInfo(console_handle, info);
}

BOOL mock_GetConsoleScreenBufferInfo(const char *file, const int line, const char *func, HANDLE console_handle,
                                     PCONSOLE_SCREEN_BUFFER_INFO info)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetConsoleScreenBufferInfo(file, line, func, console_handle, info);
    }
    else
    {
        mock_ret = delegate_real_GetConsoleScreenBufferInfo(file, line, func, console_handle, info);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetConsoleScreenBufferInfo 0x%p, 0x%p", (void *)console_handle, (void *)info);
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
