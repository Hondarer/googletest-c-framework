#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetConsoleMode(const char *file, const int line, const char *func, HANDLE console_handle,
                                  LPDWORD mode)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetConsoleMode(console_handle, mode);
}

BOOL mock_GetConsoleMode(const char *file, const int line, const char *func, HANDLE console_handle, LPDWORD mode)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->GetConsoleMode(file, line, func, console_handle, mode);
    }
    else
    {
        rtc = delegate_real_GetConsoleMode(file, line, func, console_handle, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetConsoleMode 0x%p, 0x%p", (void *)console_handle, (void *)mode);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
