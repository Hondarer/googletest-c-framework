#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_GetFileType(const char *file, const int line, const char *func, HANDLE file_handle)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetFileType(file_handle);
}

DWORD mock_GetFileType(const char *file, const int line, const char *func, HANDLE file_handle)
{
    DWORD mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetFileType(file, line, func, file_handle);
    }
    else
    {
        mock_ret = delegate_real_GetFileType(file, line, func, file_handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetFileType 0x%p", (void *)file_handle);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lu\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
