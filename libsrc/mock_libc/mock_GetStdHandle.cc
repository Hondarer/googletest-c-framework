#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

HANDLE delegate_real_GetStdHandle(const char *file, const int line, const char *func, DWORD std_handle)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetStdHandle(std_handle);
}

HANDLE mock_GetStdHandle(const char *file, const int line, const char *func, DWORD std_handle)
{
    HANDLE rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->GetStdHandle(file, line, func, std_handle);
    }
    else
    {
        rtc = delegate_real_GetStdHandle(file, line, func, std_handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetStdHandle %lu", std_handle);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
