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
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->CreateProcessW(file, line, func, application_name, command_line, process_attributes,
                                            thread_attributes, inherit_handles, creation_flags, environment,
                                            current_directory, startup_info, process_information);
    }
    else
    {
        rtc = delegate_real_CreateProcessW(file, line, func, application_name, command_line, process_attributes,
                                           thread_attributes, inherit_handles, creation_flags, environment,
                                           current_directory, startup_info, process_information);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CreateProcessW 0x%p, 0x%p", (const void *)application_name, (void *)command_line);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
