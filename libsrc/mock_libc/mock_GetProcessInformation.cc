#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetProcessInformation(const char *file, const int line, const char *func, HANDLE process, PROCESS_INFORMATION_CLASS information_class, LPVOID information, DWORD information_size)
{
    (void)file;
    (void)line;
    (void)func;

    return GetProcessInformation(process, information_class, information, information_size);
}

BOOL mock_GetProcessInformation(const char *file, const int line, const char *func, HANDLE process, PROCESS_INFORMATION_CLASS information_class, LPVOID information, DWORD information_size)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetProcessInformation(file, line, func, process, information_class, information, information_size);
    }
    else
    {
        mock_ret = delegate_real_GetProcessInformation(file, line, func, process, information_class, information, information_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetProcessInformation");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, (int)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
