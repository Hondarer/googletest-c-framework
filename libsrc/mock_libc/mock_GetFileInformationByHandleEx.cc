#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetFileInformationByHandleEx(const char *file, const int line, const char *func, HANDLE file_handle,
                                                FILE_INFO_BY_HANDLE_CLASS information_class, LPVOID information,
                                                DWORD buffer_size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetFileInformationByHandleEx(file_handle, information_class, information, buffer_size);
}

BOOL mock_GetFileInformationByHandleEx(const char *file, const int line, const char *func, HANDLE file_handle,
                                       FILE_INFO_BY_HANDLE_CLASS information_class, LPVOID information,
                                       DWORD buffer_size)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetFileInformationByHandleEx(file, line, func, file_handle, information_class,
                                                               information, buffer_size);
    }
    else
    {
        mock_ret = delegate_real_GetFileInformationByHandleEx(file, line, func, file_handle, information_class,
                                                              information, buffer_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetFileInformationByHandleEx 0x%p, %d, %lu", (void *)file_handle, (int)information_class,
               (unsigned long)buffer_size);
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
