#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_CloseHandle(const char *file, const int line, const char *func, HANDLE handle)
{
    (void)file;
    (void)line;
    (void)func;

    return CloseHandle(handle);
}

BOOL mock_CloseHandle(const char *file, const int line, const char *func, HANDLE handle)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->CloseHandle(file, line, func, handle);
    }
    else
    {
        rtc = delegate_real_CloseHandle(file, line, func, handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CloseHandle 0x%p", (void *)handle);
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
