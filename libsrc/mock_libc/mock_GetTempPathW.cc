#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_GetTempPathW(const char *file, const int line, const char *func, DWORD buffer_length,
                                 LPWSTR buffer)
{
    (void)file;
    (void)line;
    (void)func;

    return GetTempPathW(buffer_length, buffer);
}

DWORD mock_GetTempPathW(const char *file, const int line, const char *func, DWORD buffer_length, LPWSTR buffer)
{
    DWORD mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetTempPathW(file, line, func, buffer_length, buffer);
    }
    else
    {
        mock_ret = delegate_real_GetTempPathW(file, line, func, buffer_length, buffer);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetTempPathW %lu, 0x%p", buffer_length, (void *)buffer);
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
