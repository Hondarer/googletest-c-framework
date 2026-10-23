#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_ReadConsoleW(const char *file, const int line, const char *func, HANDLE file_handle, LPVOID buffer,
                                DWORD chars_to_read, LPDWORD chars_read, PCONSOLE_READCONSOLE_CONTROL control)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ReadConsoleW(file_handle, buffer, chars_to_read, chars_read, control);
}

BOOL mock_ReadConsoleW(const char *file, const int line, const char *func, HANDLE file_handle, LPVOID buffer,
                       DWORD chars_to_read, LPDWORD chars_read, PCONSOLE_READCONSOLE_CONTROL control)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret =
            _mock_windows->ReadConsoleW(file, line, func, file_handle, buffer, chars_to_read, chars_read, control);
    }
    else
    {
        mock_ret =
            delegate_real_ReadConsoleW(file, line, func, file_handle, buffer, chars_to_read, chars_read, control);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ReadConsoleW 0x%p, 0x%p, %lu, 0x%p, 0x%p", (void *)file_handle, buffer, chars_to_read,
               (void *)chars_read, (void *)control);
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
