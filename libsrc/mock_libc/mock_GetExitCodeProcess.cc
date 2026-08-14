#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetExitCodeProcess(const char *file, const int line, const char *func, HANDLE process,
                                      LPDWORD exit_code)
{
    (void)file;
    (void)line;
    (void)func;

    return GetExitCodeProcess(process, exit_code);
}

BOOL mock_GetExitCodeProcess(const char *file, const int line, const char *func, HANDLE process, LPDWORD exit_code)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetExitCodeProcess(file, line, func, process, exit_code);
    }
    else
    {
        mock_ret = delegate_real_GetExitCodeProcess(file, line, func, process, exit_code);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetExitCodeProcess 0x%p", (void *)process);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
