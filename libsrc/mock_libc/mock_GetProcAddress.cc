#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

FARPROC delegate_real_GetProcAddress(const char *file, const int line, const char *func, HMODULE module,
                                     LPCSTR proc_name)
{
    (void)file;
    (void)line;
    (void)func;

    return GetProcAddress(module, proc_name);
}

FARPROC mock_GetProcAddress(const char *file, const int line, const char *func, HMODULE module, LPCSTR proc_name)
{
    FARPROC mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetProcAddress(file, line, func, module, proc_name);
    }
    else
    {
        mock_ret = delegate_real_GetProcAddress(file, line, func, module, proc_name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        if (proc_name == nullptr)
        {
            printf("  > GetProcAddress 0x%p, (null)", (void *)module);
        }
        else if (IS_INTRESOURCE(proc_name))
        {
            printf("  > GetProcAddress 0x%p, #%lu", (void *)module, (unsigned long)(ULONG_PTR)proc_name);
        }
        else
        {
            printf("  > GetProcAddress 0x%p, %s", (void *)module, proc_name);
        }
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
