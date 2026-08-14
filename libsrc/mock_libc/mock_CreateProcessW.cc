#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_CreateProcessW(const char *file, const int line, const char *func, LPCWSTR application_name,
                                  LPWSTR command_line, LPSECURITY_ATTRIBUTES process_attributes,
                                  LPSECURITY_ATTRIBUTES thread_attributes, BOOL inherit_handles, DWORD creation_flags,
                                  LPVOID environment, LPCWSTR current_directory, LPSTARTUPINFOW startup_info,
                                  LPPROCESS_INFORMATION process_information)
{
    (void)file;
    (void)line;
    (void)func;

    return CreateProcessW(application_name, command_line, process_attributes, thread_attributes, inherit_handles,
                          creation_flags, environment, current_directory, startup_info, process_information);
}

BOOL mock_CreateProcessW(const char *file, const int line, const char *func, LPCWSTR application_name,
                         LPWSTR command_line, LPSECURITY_ATTRIBUTES process_attributes,
                         LPSECURITY_ATTRIBUTES thread_attributes, BOOL inherit_handles, DWORD creation_flags,
                         LPVOID environment, LPCWSTR current_directory, LPSTARTUPINFOW startup_info,
                         LPPROCESS_INFORMATION process_information)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->CreateProcessW(file, line, func, application_name, command_line, process_attributes,
                                            thread_attributes, inherit_handles, creation_flags, environment,
                                            current_directory, startup_info, process_information);
    }
    else
    {
        mock_ret = delegate_real_CreateProcessW(file, line, func, application_name, command_line, process_attributes,
                                           thread_attributes, inherit_handles, creation_flags, environment,
                                           current_directory, startup_info, process_information);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CreateProcessW 0x%p, 0x%p", (const void *)application_name, (void *)command_line);
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
