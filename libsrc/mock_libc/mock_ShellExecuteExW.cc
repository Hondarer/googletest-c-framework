#ifdef _WIN32

    #include <mock_shellapi.h>
    #include <test_com.h>

    #pragma comment(lib, "Shell32.lib")

using namespace testing;

BOOL delegate_real_ShellExecuteExW(const char *file, const int line, const char *func, SHELLEXECUTEINFOW *exec_info)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ShellExecuteExW(exec_info);
}

BOOL mock_ShellExecuteExW(const char *file, const int line, const char *func, SHELLEXECUTEINFOW *exec_info)
{
    BOOL mock_ret;

    if (_mock_shellapi != nullptr)
    {
        mock_ret = _mock_shellapi->ShellExecuteExW(file, line, func, exec_info);
    }
    else
    {
        mock_ret = delegate_real_ShellExecuteExW(file, line, func, exec_info);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ShellExecuteExW 0x%p", (void *)exec_info);
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
