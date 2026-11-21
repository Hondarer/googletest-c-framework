#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_WriteConsoleW(const char *file, const int line, const char *func, HANDLE console_output,
                                 const VOID *buffer, DWORD chars_to_write, LPDWORD chars_written, LPVOID reserved)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WriteConsoleW(console_output, buffer, chars_to_write, chars_written, reserved);
}

BOOL mock_WriteConsoleW(const char *file, const int line, const char *func, HANDLE console_output, const VOID *buffer,
                        DWORD chars_to_write, LPDWORD chars_written, LPVOID reserved)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->WriteConsoleW(file, line, func, console_output, buffer, chars_to_write, chars_written,
                                                reserved);
    }
    else
    {
        mock_ret = delegate_real_WriteConsoleW(file, line, func, console_output, buffer, chars_to_write, chars_written,
                                               reserved);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > WriteConsoleW 0x%p, 0x%p, %lu, 0x%p, 0x%p", (void *)console_output, buffer, chars_to_write,
               (void *)chars_written, reserved);
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
