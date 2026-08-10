#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_WaitForSingleObject(const char *file, const int line, const char *func, HANDLE handle,
                                        DWORD milliseconds)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WaitForSingleObject(handle, milliseconds);
}

DWORD mock_WaitForSingleObject(const char *file, const int line, const char *func, HANDLE handle, DWORD milliseconds)
{
    DWORD rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->WaitForSingleObject(file, line, func, handle, milliseconds);
    }
    else
    {
        rtc = delegate_real_WaitForSingleObject(file, line, func, handle, milliseconds);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > WaitForSingleObject 0x%p, %lu", (void *)handle, milliseconds);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lu\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
