#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_WaitForSingleObject(const char *file, const int line, const char *func, HANDLE handle,
                                        DWORD milliseconds)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WaitForSingleObject(handle, milliseconds);
}

DWORD mock_WaitForSingleObject(const char *file, const int line, const char *func, HANDLE handle, DWORD milliseconds)
{
    DWORD mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->WaitForSingleObject(file, line, func, handle, milliseconds);
    }
    else
    {
        mock_ret = delegate_real_WaitForSingleObject(file, line, func, handle, milliseconds);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > WaitForSingleObject 0x%p, %lu", (void *)handle, milliseconds);
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
