#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetProcessInformation(const char *file, const int line, const char *func, HANDLE process, PROCESS_INFORMATION_CLASS information_class, LPVOID information, DWORD information_size)
{
    (void)file;
    (void)line;
    (void)func;

    return SetProcessInformation(process, information_class, information, information_size);
}

BOOL mock_SetProcessInformation(const char *file, const int line, const char *func, HANDLE process, PROCESS_INFORMATION_CLASS information_class, LPVOID information, DWORD information_size)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetProcessInformation(file, line, func, process, information_class, information, information_size);
    }
    else
    {
        mock_ret = delegate_real_SetProcessInformation(file, line, func, process, information_class, information, information_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetProcessInformation");
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
