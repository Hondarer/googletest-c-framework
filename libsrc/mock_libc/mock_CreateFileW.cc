#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

HANDLE delegate_real_CreateFileW(const char *file, const int line, const char *func, LPCWSTR file_name,
                                 DWORD desired_access, DWORD share_mode, LPSECURITY_ATTRIBUTES security_attributes,
                                 DWORD creation_disposition, DWORD flags_and_attributes, HANDLE template_file)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return CreateFileW(file_name, desired_access, share_mode, security_attributes, creation_disposition,
                       flags_and_attributes, template_file);
}

HANDLE mock_CreateFileW(const char *file, const int line, const char *func, LPCWSTR file_name, DWORD desired_access,
                        DWORD share_mode, LPSECURITY_ATTRIBUTES security_attributes, DWORD creation_disposition,
                        DWORD flags_and_attributes, HANDLE template_file)
{
    HANDLE mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret =
            _mock_windows->CreateFileW(file, line, func, file_name, desired_access, share_mode, security_attributes,
                                       creation_disposition, flags_and_attributes, template_file);
    }
    else
    {
        mock_ret =
            delegate_real_CreateFileW(file, line, func, file_name, desired_access, share_mode, security_attributes,
                                      creation_disposition, flags_and_attributes, template_file);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CreateFileW %ls, 0x%lx, 0x%lx, %lu", file_name != nullptr ? file_name : L"(null)", desired_access,
               share_mode, creation_disposition);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
