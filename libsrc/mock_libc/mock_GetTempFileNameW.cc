#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

UINT delegate_real_GetTempFileNameW(const char *file, const int line, const char *func, LPCWSTR path_name,
                                    LPCWSTR prefix_string, UINT unique, LPWSTR temp_file_name)
{
    (void)file;
    (void)line;
    (void)func;

    return GetTempFileNameW(path_name, prefix_string, unique, temp_file_name);
}

UINT mock_GetTempFileNameW(const char *file, const int line, const char *func, LPCWSTR path_name,
                           LPCWSTR prefix_string, UINT unique, LPWSTR temp_file_name)
{
    UINT mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetTempFileNameW(file, line, func, path_name, prefix_string, unique, temp_file_name);
    }
    else
    {
        mock_ret =
            delegate_real_GetTempFileNameW(file, line, func, path_name, prefix_string, unique, temp_file_name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetTempFileNameW %ls, %ls, %u, 0x%p", path_name != nullptr ? path_name : L"(null)",
               prefix_string != nullptr ? prefix_string : L"(null)", unique, (void *)temp_file_name);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %u\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
