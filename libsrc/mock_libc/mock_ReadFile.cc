#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_ReadFile(const char *file, const int line, const char *func, HANDLE file_handle, LPVOID buffer,
                            DWORD bytes_to_read, LPDWORD bytes_read, LPOVERLAPPED overlapped)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ReadFile(file_handle, buffer, bytes_to_read, bytes_read, overlapped);
}

BOOL mock_ReadFile(const char *file, const int line, const char *func, HANDLE file_handle, LPVOID buffer,
                   DWORD bytes_to_read, LPDWORD bytes_read, LPOVERLAPPED overlapped)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->ReadFile(file, line, func, file_handle, buffer, bytes_to_read, bytes_read, overlapped);
    }
    else
    {
        rtc = delegate_real_ReadFile(file, line, func, file_handle, buffer, bytes_to_read, bytes_read, overlapped);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ReadFile 0x%p, 0x%p, %lu, 0x%p, 0x%p", (void *)file_handle, buffer, bytes_to_read,
               (void *)bytes_read, (void *)overlapped);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
