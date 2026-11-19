#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetStdHandle(const char *file, const int line, const char *func, DWORD std_handle, HANDLE handle)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return SetStdHandle(std_handle, handle);
}

BOOL mock_SetStdHandle(const char *file, const int line, const char *func, DWORD std_handle, HANDLE handle)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetStdHandle(file, line, func, std_handle, handle);
    }
    else
    {
        mock_ret = delegate_real_SetStdHandle(file, line, func, std_handle, handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetStdHandle %lu, 0x%p", std_handle, (void *)handle);
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
