#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_GetLastError(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;

    return GetLastError();
}

DWORD mock_GetLastError(const char *file, const int line, const char *func)
{
    DWORD mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetLastError(file, line, func);
    }
    else
    {
        mock_ret = delegate_real_GetLastError(file, line, func);
    }

    if (getTraceLevel() >= TRACE_DETAIL)
    {
        printf("  > GetLastError from %s:%d -> %lu\n", file, line, mock_ret);
    }

    return mock_ret;
}

#endif // _WIN32
