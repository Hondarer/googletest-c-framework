#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_GetModuleFileNameW(const char *file, const int line, const char *func, HMODULE module,
                                       LPWSTR filename, DWORD size)
{
    (void)file;
    (void)line;
    (void)func;

    return GetModuleFileNameW(module, filename, size);
}

DWORD mock_GetModuleFileNameW(const char *file, const int line, const char *func, HMODULE module, LPWSTR filename,
                              DWORD size)
{
    DWORD rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->GetModuleFileNameW(file, line, func, module, filename, size);
    }
    else
    {
        rtc = delegate_real_GetModuleFileNameW(file, line, func, module, filename, size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetModuleFileNameW 0x%p, %lu", (void *)module, size);
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
