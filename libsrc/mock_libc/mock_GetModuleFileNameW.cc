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
    DWORD mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetModuleFileNameW(file, line, func, module, filename, size);
    }
    else
    {
        mock_ret = delegate_real_GetModuleFileNameW(file, line, func, module, filename, size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetModuleFileNameW 0x%p, %lu", (void *)module, size);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lu\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
