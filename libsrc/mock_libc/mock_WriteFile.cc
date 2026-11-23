#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_WriteFile(const char *file, const int line, const char *func, HANDLE file_handle,
                             LPCVOID buffer, DWORD bytes_to_write, LPDWORD bytes_written,
                             LPOVERLAPPED overlapped)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WriteFile(file_handle, buffer, bytes_to_write, bytes_written, overlapped);
}

BOOL mock_WriteFile(const char *file, const int line, const char *func, HANDLE file_handle, LPCVOID buffer,
                    DWORD bytes_to_write, LPDWORD bytes_written, LPOVERLAPPED overlapped)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->WriteFile(file, line, func, file_handle, buffer, bytes_to_write, bytes_written,
                                            overlapped);
    }
    else
    {
        mock_ret = delegate_real_WriteFile(file, line, func, file_handle, buffer, bytes_to_write, bytes_written,
                                          overlapped);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > WriteFile 0x%p, 0x%p, %lu, 0x%p, 0x%p", (void *)file_handle, buffer, bytes_to_write,
               (void *)bytes_written, (void *)overlapped);
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
