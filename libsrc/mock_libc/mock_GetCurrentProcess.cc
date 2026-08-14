#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

HANDLE delegate_real_GetCurrentProcess(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;

    return GetCurrentProcess();
}

HANDLE mock_GetCurrentProcess(const char *file, const int line, const char *func)
{
    HANDLE rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->GetCurrentProcess(file, line, func);
    }
    else
    {
        rtc = delegate_real_GetCurrentProcess(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetCurrentProcess");
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
