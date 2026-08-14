#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_TerminateProcess(const char *file, const int line, const char *func, HANDLE process, UINT exit_code)
{
    (void)file;
    (void)line;
    (void)func;

    return TerminateProcess(process, exit_code);
}

BOOL mock_TerminateProcess(const char *file, const int line, const char *func, HANDLE process, UINT exit_code)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->TerminateProcess(file, line, func, process, exit_code);
    }
    else
    {
        mock_ret = delegate_real_TerminateProcess(file, line, func, process, exit_code);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > TerminateProcess 0x%p, %u", (void *)process, exit_code);
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
