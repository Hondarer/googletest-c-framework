#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetFileTime(const char *file, const int line, const char *func, HANDLE file_handle,
                               const FILETIME *creation, const FILETIME *last_access, const FILETIME *last_write)
{
    (void)file;
    (void)line;
    (void)func;

    return SetFileTime(file_handle, creation, last_access, last_write);
}

BOOL mock_SetFileTime(const char *file, const int line, const char *func, HANDLE file_handle, const FILETIME *creation,
                      const FILETIME *last_access, const FILETIME *last_write)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetFileTime(file, line, func, file_handle, creation, last_access, last_write);
    }
    else
    {
        mock_ret = delegate_real_SetFileTime(file, line, func, file_handle, creation, last_access, last_write);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetFileTime 0x%p", (void *)file_handle);
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
