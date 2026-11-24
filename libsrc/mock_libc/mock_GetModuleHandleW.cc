#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

HMODULE delegate_real_GetModuleHandleW(const char *file, const int line, const char *func, LPCWSTR module_name)
{
    (void)file;
    (void)line;
    (void)func;

    return GetModuleHandleW(module_name);
}

HMODULE mock_GetModuleHandleW(const char *file, const int line, const char *func, LPCWSTR module_name)
{
    HMODULE mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetModuleHandleW(file, line, func, module_name);
    }
    else
    {
        mock_ret = delegate_real_GetModuleHandleW(file, line, func, module_name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetModuleHandleW %ls", module_name != nullptr ? module_name : L"(null)");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
