#ifdef _WIN32

    #ifndef _MOCK_WINDOWS_H
        #define _MOCK_WINDOWS_H

        /* winsock2.h を windows.h より先に取り込み、winsock.h との衝突を防ぐ。 */
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif /* WIN32_LEAN_AND_MEAN */
        #include <winsock2.h>
        #include <ws2tcpip.h>
        #include <windows.h>
        #include <evntrace.h>
        #ifdef byte
            #undef byte /* C++17 std::byte と Windows SDK byte typedef の競合を解消 */
        #endif          /* byte */

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern ULONGLONG mock_GetTickCount64(const char *, const int, const char *);
    extern BOOL mock_QueryPerformanceFrequency(const char *, const int, const char *, LARGE_INTEGER *);
    extern BOOL mock_QueryPerformanceCounter(const char *, const int, const char *, LARGE_INTEGER *);
    extern void mock_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
    extern HANDLE mock_GetStdHandle(const char *, const int, const char *, DWORD);
    extern BOOL mock_GetConsoleMode(const char *, const int, const char *, HANDLE, LPDWORD);
    extern BOOL mock_GetConsoleScreenBufferInfo(const char *, const int, const char *, HANDLE,
                                                PCONSOLE_SCREEN_BUFFER_INFO);
    extern BOOL mock_SetConsoleMode(const char *, const int, const char *, HANDLE, DWORD);
    extern DWORD mock_WaitForSingleObject(const char *, const int, const char *, HANDLE, DWORD);
    extern BOOL mock_ReadConsoleW(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD,
                                  PCONSOLE_READCONSOLE_CONTROL);
    extern BOOL mock_WriteConsoleW(const char *, const int, const char *, HANDLE, const VOID *, DWORD, LPDWORD, LPVOID);
    extern BOOL mock_ReadFile(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
    extern BOOL mock_WriteFile(const char *, const int, const char *, HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
    extern HANDLE mock_CreateFileMappingA(const char *, const int, const char *, HANDLE, LPSECURITY_ATTRIBUTES, DWORD,
                                          DWORD, DWORD, LPCSTR);
    extern LPVOID mock_MapViewOfFile(const char *, const int, const char *, HANDLE, DWORD, DWORD, DWORD, SIZE_T);
    extern BOOL mock_UnmapViewOfFile(const char *, const int, const char *, LPCVOID);
    extern BOOL mock_FlushViewOfFile(const char *, const int, const char *, LPCVOID, SIZE_T);
    extern BOOL mock_FlushFileBuffers(const char *, const int, const char *, HANDLE);
    extern BOOL mock_GetFileTime(const char *, const int, const char *, HANDLE, LPFILETIME, LPFILETIME, LPFILETIME);
    extern BOOL mock_SetFileTime(const char *, const int, const char *, HANDLE, const FILETIME *, const FILETIME *,
                                 const FILETIME *);
    extern BOOL mock_CloseHandle(const char *, const int, const char *, HANDLE);
    extern DWORD mock_GetLastError(const char *, const int, const char *);
    extern DWORD mock_GetModuleFileNameW(const char *, const int, const char *, HMODULE, LPWSTR, DWORD);
    extern BOOL mock_GetComputerNameExW(const char *, const int, const char *, COMPUTER_NAME_FORMAT, LPWSTR, LPDWORD);
    extern BOOL mock_GetUserPreferredUILanguages(const char *, const int, const char *, DWORD, PULONG, PZZWSTR, PULONG);
    extern int mock_GetUserDefaultLocaleName(const char *, const int, const char *, LPWSTR, int);
    extern BOOL mock_GetExitCodeProcess(const char *, const int, const char *, HANDLE, LPDWORD);
    extern BOOL mock_TerminateProcess(const char *, const int, const char *, HANDLE, UINT);
    extern BOOL mock_InitializeProcThreadAttributeList(const char *, const int, const char *,
                                                       LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
    extern BOOL mock_UpdateProcThreadAttribute(const char *, const int, const char *, LPPROC_THREAD_ATTRIBUTE_LIST,
                                               DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
    extern void mock_DeleteProcThreadAttributeList(const char *, const int, const char *, LPPROC_THREAD_ATTRIBUTE_LIST);
    extern BOOL mock_CreateProcessW(const char *, const int, const char *, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                                    LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW,
                                    LPPROCESS_INFORMATION);
    extern HANDLE mock_GetCurrentProcess(const char *, const int, const char *);
    extern DWORD mock_GetCurrentProcessId(const char *, const int, const char *);
    extern BOOL mock_DuplicateHandle(const char *, const int, const char *, HANDLE, HANDLE, HANDLE, LPHANDLE, DWORD,
                                     BOOL, DWORD);
    extern HANDLE mock_OpenProcess(const char *, const int, const char *, DWORD, BOOL, DWORD);
    extern HANDLE mock_OpenThread(const char *, const int, const char *, DWORD, BOOL, DWORD);
    extern BOOL mock_SetPriorityClass(const char *, const int, const char *, HANDLE, DWORD);
    extern DWORD mock_GetPriorityClass(const char *, const int, const char *, HANDLE);
    extern BOOL mock_SetProcessInformation(const char *, const int, const char *, HANDLE, PROCESS_INFORMATION_CLASS,
                                           LPVOID, DWORD);
    extern BOOL mock_GetProcessInformation(const char *, const int, const char *, HANDLE, PROCESS_INFORMATION_CLASS,
                                           LPVOID, DWORD);
    extern BOOL mock_SetProcessPriorityBoost(const char *, const int, const char *, HANDLE, BOOL);
    extern BOOL mock_GetProcessPriorityBoost(const char *, const int, const char *, HANDLE, PBOOL);
    extern BOOL mock_SetThreadPriority(const char *, const int, const char *, HANDLE, int);
    extern int mock_GetThreadPriority(const char *, const int, const char *, HANDLE);
    extern BOOL mock_SetStdHandle(const char *, const int, const char *, DWORD, HANDLE);
    extern DWORD mock_GetFileType(const char *, const int, const char *, HANDLE);
    extern HANDLE mock_CreateFileW(const char *, const int, const char *, LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES,
                                   DWORD, DWORD, HANDLE);
    extern BOOL mock_SetServiceStatus(const char *, const int, const char *, SERVICE_STATUS_HANDLE, LPSERVICE_STATUS);
    extern BOOL mock_SetConsoleCtrlHandler(const char *, const int, const char *, PHANDLER_ROUTINE, BOOL);
    extern UINT mock_GetConsoleCP(const char *, const int, const char *);
    extern UINT mock_GetConsoleOutputCP(const char *, const int, const char *);
    extern BOOL mock_SetConsoleCP(const char *, const int, const char *, UINT);
    extern BOOL mock_SetConsoleOutputCP(const char *, const int, const char *, UINT);
    extern BOOL mock_AttachConsole(const char *, const int, const char *, DWORD);
    extern BOOL mock_FreeConsole(const char *, const int, const char *);
    extern int mock_MultiByteToWideChar(const char *, const int, const char *, UINT, DWORD, LPCCH, int, LPWSTR, int);
    extern int mock_WideCharToMultiByte(const char *, const int, const char *, UINT, DWORD, LPCWCH, int, LPSTR, int,
                                        LPCCH, LPBOOL);
    extern void mock_OutputDebugStringW(const char *, const int, const char *, LPCWSTR);
    extern BOOL mock_SetHandleInformation(const char *, const int, const char *, HANDLE, DWORD, DWORD);
    extern BOOL mock_SetEvent(const char *, const int, const char *, HANDLE);
    extern BOOL mock_ResetEvent(const char *, const int, const char *, HANDLE);
    extern BOOL mock_GetTokenInformation(const char *, const int, const char *, HANDLE, TOKEN_INFORMATION_CLASS, LPVOID,
                                         DWORD, PDWORD);
    extern BOOL mock_ProcessIdToSessionId(const char *, const int, const char *, DWORD, DWORD *);
    extern BOOL mock_GetNamedPipeClientProcessId(const char *, const int, const char *, HANDLE, PULONG);
    extern DWORD mock_GetTempPathW(const char *, const int, const char *, DWORD, LPWSTR);
    extern UINT mock_GetTempFileNameW(const char *, const int, const char *, LPCWSTR, LPCWSTR, UINT, LPWSTR);
    extern HMODULE mock_GetModuleHandleW(const char *, const int, const char *, LPCWSTR);
    extern FARPROC mock_GetProcAddress(const char *, const int, const char *, HMODULE, LPCSTR);
    extern int mock_CompareStringOrdinal(const char *, const int, const char *, LPCWCH, int, LPCWCH, int, BOOL);
    extern ULONG mock_StartTraceW(const char *, const int, const char *, CONTROLTRACE_ID *, LPCWSTR,
                                  PEVENT_TRACE_PROPERTIES);
    extern ULONG mock_ControlTraceW(const char *, const int, const char *, CONTROLTRACE_ID, LPCWSTR,
                                    PEVENT_TRACE_PROPERTIES, ULONG);
    extern ULONG mock_EnableTraceEx2(const char *, const int, const char *, CONTROLTRACE_ID, LPCGUID, ULONG, UCHAR,
                                     ULONGLONG, ULONGLONG, ULONG, PENABLE_TRACE_PARAMETERS);
    extern PROCESSTRACE_HANDLE mock_OpenTraceW(const char *, const int, const char *, PEVENT_TRACE_LOGFILEW);
    extern ULONG mock_ProcessTrace(const char *, const int, const char *, PROCESSTRACE_HANDLE *, ULONG, LPFILETIME,
                                   LPFILETIME);
    extern ULONG mock_CloseTrace(const char *, const int, const char *, PROCESSTRACE_HANDLE);

        #ifdef __cplusplus
}
        #endif

        #ifdef _IN_OVERRIDE_HEADER_WINDOWS_H

            #define GetTickCount64() mock_GetTickCount64(__FILE__, __LINE__, __func__)
            #define QueryPerformanceFrequency(frequency) \
                mock_QueryPerformanceFrequency(__FILE__, __LINE__, __func__, frequency)
            #define QueryPerformanceCounter(counter) mock_QueryPerformanceCounter(__FILE__, __LINE__, __func__, counter)
            #define GetSystemTimeAsFileTime(file_time) \
                mock_GetSystemTimeAsFileTime(__FILE__, __LINE__, __func__, file_time)
            #define GetStdHandle(std_handle) mock_GetStdHandle(__FILE__, __LINE__, __func__, std_handle)
            #define GetConsoleMode(console_handle, mode) \
                mock_GetConsoleMode(__FILE__, __LINE__, __func__, console_handle, mode)
            #define GetConsoleScreenBufferInfo(console_handle, info) \
                mock_GetConsoleScreenBufferInfo(__FILE__, __LINE__, __func__, console_handle, info)
            #define SetConsoleMode(console_handle, mode) \
                mock_SetConsoleMode(__FILE__, __LINE__, __func__, console_handle, mode)
            #define WaitForSingleObject(handle, milliseconds) \
                mock_WaitForSingleObject(__FILE__, __LINE__, __func__, handle, milliseconds)
            #define ReadConsoleW(handle, buffer, count, read, control) \
                mock_ReadConsoleW(__FILE__, __LINE__, __func__, handle, buffer, count, read, control)
            #define WriteConsoleW(console_output, buffer, chars_to_write, chars_written, reserved) \
                mock_WriteConsoleW(__FILE__, __LINE__, __func__, console_output, buffer, chars_to_write, \
                                   chars_written, reserved)
            #define ReadFile(file_handle, buffer, bytes_to_read, bytes_read, overlapped) \
                mock_ReadFile(__FILE__, __LINE__, __func__, file_handle, buffer, bytes_to_read, bytes_read, overlapped)
            #define WriteFile(file_handle, buffer, bytes_to_write, bytes_written, overlapped) \
                mock_WriteFile(__FILE__, __LINE__, __func__, file_handle, buffer, bytes_to_write, bytes_written, \
                               overlapped)
            #define CreateFileMappingA(file, attributes, protect, size_high, size_low, name) \
                mock_CreateFileMappingA(__FILE__, __LINE__, __func__, file, attributes, protect, size_high, size_low, \
                                        name)
            #define MapViewOfFile(mapping, access, offset_high, offset_low, bytes) \
                mock_MapViewOfFile(__FILE__, __LINE__, __func__, mapping, access, offset_high, offset_low, bytes)
            #define UnmapViewOfFile(address)        mock_UnmapViewOfFile(__FILE__, __LINE__, __func__, address)
            #define FlushViewOfFile(address, bytes) mock_FlushViewOfFile(__FILE__, __LINE__, __func__, address, bytes)
            #define FlushFileBuffers(file)          mock_FlushFileBuffers(__FILE__, __LINE__, __func__, file)
            #define GetFileTime(file_handle, creation, last_access, last_write) \
                mock_GetFileTime(__FILE__, __LINE__, __func__, file_handle, creation, last_access, last_write)
            #define SetFileTime(file_handle, creation, last_access, last_write) \
                mock_SetFileTime(__FILE__, __LINE__, __func__, file_handle, creation, last_access, last_write)
            #define CloseHandle(handle) mock_CloseHandle(__FILE__, __LINE__, __func__, handle)
            #define GetLastError()      mock_GetLastError(__FILE__, __LINE__, __func__)
            #define GetModuleFileNameW(module, filename, size) \
                mock_GetModuleFileNameW(__FILE__, __LINE__, __func__, module, filename, size)
            #define GetComputerNameExW(name_type, buffer, size) \
                mock_GetComputerNameExW(__FILE__, __LINE__, __func__, name_type, buffer, size)
            #define GetUserPreferredUILanguages(flags, language_count, languages, language_size) \
                mock_GetUserPreferredUILanguages(__FILE__, __LINE__, __func__, flags, language_count, languages, \
                                                 language_size)
            #define GetUserDefaultLocaleName(locale_name, locale_name_count) \
                mock_GetUserDefaultLocaleName(__FILE__, __LINE__, __func__, locale_name, locale_name_count)
            #define GetExitCodeProcess(process, exit_code) \
                mock_GetExitCodeProcess(__FILE__, __LINE__, __func__, process, exit_code)
            #define TerminateProcess(process, exit_code) \
                mock_TerminateProcess(__FILE__, __LINE__, __func__, process, exit_code)
            #define InitializeProcThreadAttributeList(attribute_list, attribute_count, flags, size) \
                mock_InitializeProcThreadAttributeList(__FILE__, __LINE__, __func__, attribute_list, attribute_count, \
                                                       flags, size)
            #define UpdateProcThreadAttribute(attribute_list, flags, attribute, value, size, previous, return_size) \
                mock_UpdateProcThreadAttribute(__FILE__, __LINE__, __func__, attribute_list, flags, attribute, value, \
                                               size, previous, return_size)
            #define DeleteProcThreadAttributeList(attribute_list) \
                mock_DeleteProcThreadAttributeList(__FILE__, __LINE__, __func__, attribute_list)
            #define CreateProcessW(application_name, command_line, process_attributes, thread_attributes, \
                                   inherit_handles, creation_flags, environment, current_directory, startup_info, \
                                   process_information) \
                mock_CreateProcessW(__FILE__, __LINE__, __func__, application_name, command_line, process_attributes, \
                                    thread_attributes, inherit_handles, creation_flags, environment, \
                                    current_directory, startup_info, process_information)
            #define GetCurrentProcess()   mock_GetCurrentProcess(__FILE__, __LINE__, __func__)
            #define GetCurrentProcessId() mock_GetCurrentProcessId(__FILE__, __LINE__, __func__)
            #define DuplicateHandle(source_process, source_handle, target_process, target_handle, desired_access, \
                                    inherit_handle, options) \
                mock_DuplicateHandle(__FILE__, __LINE__, __func__, source_process, source_handle, target_process, \
                                     target_handle, desired_access, inherit_handle, options)
            #define OpenProcess(desired_access, inherit_handle, process_id) \
                mock_OpenProcess(__FILE__, __LINE__, __func__, desired_access, inherit_handle, process_id)
            #define OpenThread(desired_access, inherit_handle, thread_id) \
                mock_OpenThread(__FILE__, __LINE__, __func__, desired_access, inherit_handle, thread_id)
            #define SetPriorityClass(process, priority_class) \
                mock_SetPriorityClass(__FILE__, __LINE__, __func__, process, priority_class)
            #define GetPriorityClass(process) mock_GetPriorityClass(__FILE__, __LINE__, __func__, process)
            #define SetProcessInformation(process, information_class, information, information_size) \
                mock_SetProcessInformation(__FILE__, __LINE__, __func__, process, information_class, information, \
                                           information_size)
            #define GetProcessInformation(process, information_class, information, information_size) \
                mock_GetProcessInformation(__FILE__, __LINE__, __func__, process, information_class, information, \
                                           information_size)
            #define SetProcessPriorityBoost(process, disable_priority_boost) \
                mock_SetProcessPriorityBoost(__FILE__, __LINE__, __func__, process, disable_priority_boost)
            #define GetProcessPriorityBoost(process, disable_priority_boost) \
                mock_GetProcessPriorityBoost(__FILE__, __LINE__, __func__, process, disable_priority_boost)
            #define SetThreadPriority(thread, priority) \
                mock_SetThreadPriority(__FILE__, __LINE__, __func__, thread, priority)
            #define GetThreadPriority(thread)        mock_GetThreadPriority(__FILE__, __LINE__, __func__, thread)
            #define SetStdHandle(std_handle, handle) mock_SetStdHandle(__FILE__, __LINE__, __func__, std_handle, handle)
            #define GetFileType(file_handle)         mock_GetFileType(__FILE__, __LINE__, __func__, file_handle)
            #define CreateFileW(file_name, desired_access, share_mode, security_attributes, creation_disposition, \
                                flags_and_attributes, template_file) \
                mock_CreateFileW(__FILE__, __LINE__, __func__, file_name, desired_access, share_mode, \
                                 security_attributes, creation_disposition, flags_and_attributes, template_file)
            #define SetServiceStatus(service_status_handle, service_status) \
                mock_SetServiceStatus(__FILE__, __LINE__, __func__, service_status_handle, service_status)
            #define SetConsoleCtrlHandler(handler_routine, add) \
                mock_SetConsoleCtrlHandler(__FILE__, __LINE__, __func__, handler_routine, add)
            #define GetConsoleCP()                   mock_GetConsoleCP(__FILE__, __LINE__, __func__)
            #define GetConsoleOutputCP()             mock_GetConsoleOutputCP(__FILE__, __LINE__, __func__)
            #define SetConsoleCP(code_page_id)       mock_SetConsoleCP(__FILE__, __LINE__, __func__, code_page_id)
            #define SetConsoleOutputCP(code_page_id) mock_SetConsoleOutputCP(__FILE__, __LINE__, __func__, code_page_id)
            #define AttachConsole(process_id)        mock_AttachConsole(__FILE__, __LINE__, __func__, process_id)
            #define FreeConsole()                    mock_FreeConsole(__FILE__, __LINE__, __func__)
            #define MultiByteToWideChar(code_page, flags, multi_byte_str, multi_byte_count, wide_char_str, \
                                        wide_char_count) \
                mock_MultiByteToWideChar(__FILE__, __LINE__, __func__, code_page, flags, multi_byte_str, \
                                         multi_byte_count, wide_char_str, wide_char_count)
            #define WideCharToMultiByte(code_page, flags, wide_char_str, wide_char_count, multi_byte_str, \
                                        multi_byte_count, default_char, used_default_char) \
                mock_WideCharToMultiByte(__FILE__, __LINE__, __func__, code_page, flags, wide_char_str, \
                                         wide_char_count, multi_byte_str, multi_byte_count, default_char, \
                                         used_default_char)
            #define OutputDebugStringW(output_string) \
                mock_OutputDebugStringW(__FILE__, __LINE__, __func__, output_string)
            #define SetHandleInformation(object, mask, flags) \
                mock_SetHandleInformation(__FILE__, __LINE__, __func__, object, mask, flags)
            #define SetEvent(event)   mock_SetEvent(__FILE__, __LINE__, __func__, event)
            #define ResetEvent(event) mock_ResetEvent(__FILE__, __LINE__, __func__, event)
            #define GetTokenInformation(token_handle, token_information_class, token_information, \
                                        token_information_length, return_length) \
                mock_GetTokenInformation(__FILE__, __LINE__, __func__, token_handle, token_information_class, \
                                         token_information, token_information_length, return_length)
            #define ProcessIdToSessionId(process_id, session_id) \
                mock_ProcessIdToSessionId(__FILE__, __LINE__, __func__, process_id, session_id)
            #define GetNamedPipeClientProcessId(pipe, client_process_id) \
                mock_GetNamedPipeClientProcessId(__FILE__, __LINE__, __func__, pipe, client_process_id)
            #define GetTempPathW(buffer_length, buffer) \
                mock_GetTempPathW(__FILE__, __LINE__, __func__, buffer_length, buffer)
            #define GetTempFileNameW(path_name, prefix_string, unique, temp_file_name) \
                mock_GetTempFileNameW(__FILE__, __LINE__, __func__, path_name, prefix_string, unique, temp_file_name)
            #define GetModuleHandleW(module_name) mock_GetModuleHandleW(__FILE__, __LINE__, __func__, module_name)
            #define GetProcAddress(module, proc_name) \
                mock_GetProcAddress(__FILE__, __LINE__, __func__, module, proc_name)
            #define CompareStringOrdinal(string1, count1, string2, count2, ignore_case) \
                mock_CompareStringOrdinal(__FILE__, __LINE__, __func__, string1, count1, string2, count2, ignore_case)
            #define StartTraceW(trace_id, instance_name, properties) \
                mock_StartTraceW(__FILE__, __LINE__, __func__, trace_id, instance_name, properties)
            #define ControlTraceW(trace_id, instance_name, properties, control_code) \
                mock_ControlTraceW(__FILE__, __LINE__, __func__, trace_id, instance_name, properties, control_code)
            #define EnableTraceEx2(trace_id, provider_id, control_code, level, match_any_keyword, match_all_keyword, \
                                   timeout, enable_parameters) \
                mock_EnableTraceEx2(__FILE__, __LINE__, __func__, trace_id, provider_id, control_code, level, \
                                    match_any_keyword, match_all_keyword, timeout, enable_parameters)
            #define OpenTraceW(logfile) mock_OpenTraceW(__FILE__, __LINE__, __func__, logfile)
            #define ProcessTrace(handle_array, handle_count, start_time, end_time) \
                mock_ProcessTrace(__FILE__, __LINE__, __func__, handle_array, handle_count, start_time, end_time)
            #define CloseTrace(trace_handle) mock_CloseTrace(__FILE__, __LINE__, __func__, trace_handle)

        #else // _IN_OVERRIDE_HEADER_WINDOWS_H

            #include <gmock/gmock.h>

extern ULONGLONG delegate_real_GetTickCount64(const char *, const int, const char *);
extern ULONGLONG delegate_fake_GetTickCount64(const char *, const int, const char *);
extern BOOL delegate_real_QueryPerformanceFrequency(const char *, const int, const char *, LARGE_INTEGER *);
extern BOOL delegate_fake_QueryPerformanceFrequency(const char *, const int, const char *, LARGE_INTEGER *);
extern BOOL delegate_real_QueryPerformanceCounter(const char *, const int, const char *, LARGE_INTEGER *);
extern BOOL delegate_fake_QueryPerformanceCounter(const char *, const int, const char *, LARGE_INTEGER *);
extern void delegate_real_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern void delegate_fake_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern HANDLE delegate_real_GetStdHandle(const char *, const int, const char *, DWORD);
extern BOOL delegate_real_GetConsoleMode(const char *, const int, const char *, HANDLE, LPDWORD);
extern BOOL delegate_real_GetConsoleScreenBufferInfo(const char *, const int, const char *, HANDLE,
                                                     PCONSOLE_SCREEN_BUFFER_INFO);
extern BOOL delegate_real_SetConsoleMode(const char *, const int, const char *, HANDLE, DWORD);
extern DWORD delegate_real_WaitForSingleObject(const char *, const int, const char *, HANDLE, DWORD);
extern BOOL delegate_real_ReadConsoleW(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD,
                                       PCONSOLE_READCONSOLE_CONTROL);
extern BOOL delegate_real_WriteConsoleW(const char *, const int, const char *, HANDLE, const VOID *, DWORD, LPDWORD,
                                        LPVOID);
extern BOOL delegate_real_ReadFile(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
extern BOOL delegate_real_WriteFile(const char *, const int, const char *, HANDLE, LPCVOID, DWORD, LPDWORD,
                                    LPOVERLAPPED);
extern HANDLE delegate_real_CreateFileMappingA(const char *, const int, const char *, HANDLE, LPSECURITY_ATTRIBUTES,
                                               DWORD, DWORD, DWORD, LPCSTR);
extern LPVOID delegate_real_MapViewOfFile(const char *, const int, const char *, HANDLE, DWORD, DWORD, DWORD, SIZE_T);
extern BOOL delegate_real_UnmapViewOfFile(const char *, const int, const char *, LPCVOID);
extern BOOL delegate_real_FlushViewOfFile(const char *, const int, const char *, LPCVOID, SIZE_T);
extern BOOL delegate_real_FlushFileBuffers(const char *, const int, const char *, HANDLE);
extern BOOL delegate_real_GetFileTime(const char *, const int, const char *, HANDLE, LPFILETIME, LPFILETIME,
                                      LPFILETIME);
extern BOOL delegate_real_SetFileTime(const char *, const int, const char *, HANDLE, const FILETIME *, const FILETIME *,
                                      const FILETIME *);
extern BOOL delegate_real_CloseHandle(const char *, const int, const char *, HANDLE);
extern DWORD delegate_real_GetLastError(const char *, const int, const char *);
extern DWORD delegate_real_GetModuleFileNameW(const char *, const int, const char *, HMODULE, LPWSTR, DWORD);
extern BOOL delegate_real_GetComputerNameExW(const char *, const int, const char *, COMPUTER_NAME_FORMAT, LPWSTR,
                                             LPDWORD);
extern BOOL delegate_real_GetUserPreferredUILanguages(const char *, const int, const char *, DWORD, PULONG, PZZWSTR,
                                                      PULONG);
extern int delegate_real_GetUserDefaultLocaleName(const char *, const int, const char *, LPWSTR, int);
extern BOOL delegate_real_GetExitCodeProcess(const char *, const int, const char *, HANDLE, LPDWORD);
extern BOOL delegate_real_TerminateProcess(const char *, const int, const char *, HANDLE, UINT);
extern BOOL delegate_real_InitializeProcThreadAttributeList(const char *, const int, const char *,
                                                            LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
extern BOOL delegate_real_UpdateProcThreadAttribute(const char *, const int, const char *, LPPROC_THREAD_ATTRIBUTE_LIST,
                                                    DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
extern void delegate_real_DeleteProcThreadAttributeList(const char *, const int, const char *,
                                                        LPPROC_THREAD_ATTRIBUTE_LIST);
extern BOOL delegate_real_CreateProcessW(const char *, const int, const char *, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                                         LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW,
                                         LPPROCESS_INFORMATION);
extern HANDLE delegate_real_GetCurrentProcess(const char *, const int, const char *);
extern DWORD delegate_real_GetCurrentProcessId(const char *, const int, const char *);
extern BOOL delegate_real_DuplicateHandle(const char *, const int, const char *, HANDLE, HANDLE, HANDLE, LPHANDLE,
                                          DWORD, BOOL, DWORD);
extern HANDLE delegate_real_OpenProcess(const char *, const int, const char *, DWORD, BOOL, DWORD);
extern HANDLE delegate_real_OpenThread(const char *, const int, const char *, DWORD, BOOL, DWORD);
extern BOOL delegate_real_SetPriorityClass(const char *, const int, const char *, HANDLE, DWORD);
extern DWORD delegate_real_GetPriorityClass(const char *, const int, const char *, HANDLE);
extern BOOL delegate_real_SetProcessInformation(const char *, const int, const char *, HANDLE,
                                                PROCESS_INFORMATION_CLASS, LPVOID, DWORD);
extern BOOL delegate_real_GetProcessInformation(const char *, const int, const char *, HANDLE,
                                                PROCESS_INFORMATION_CLASS, LPVOID, DWORD);
extern BOOL delegate_real_SetProcessPriorityBoost(const char *, const int, const char *, HANDLE, BOOL);
extern BOOL delegate_real_GetProcessPriorityBoost(const char *, const int, const char *, HANDLE, PBOOL);
extern BOOL delegate_real_SetThreadPriority(const char *, const int, const char *, HANDLE, int);
extern int delegate_real_GetThreadPriority(const char *, const int, const char *, HANDLE);
extern BOOL delegate_real_SetStdHandle(const char *, const int, const char *, DWORD, HANDLE);
extern DWORD delegate_real_GetFileType(const char *, const int, const char *, HANDLE);
extern HANDLE delegate_real_CreateFileW(const char *, const int, const char *, LPCWSTR, DWORD, DWORD,
                                        LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
extern BOOL delegate_real_SetServiceStatus(const char *, const int, const char *, SERVICE_STATUS_HANDLE,
                                           LPSERVICE_STATUS);
extern BOOL delegate_real_SetConsoleCtrlHandler(const char *, const int, const char *, PHANDLER_ROUTINE, BOOL);
extern UINT delegate_real_GetConsoleCP(const char *, const int, const char *);
extern UINT delegate_real_GetConsoleOutputCP(const char *, const int, const char *);
extern BOOL delegate_real_SetConsoleCP(const char *, const int, const char *, UINT);
extern BOOL delegate_real_SetConsoleOutputCP(const char *, const int, const char *, UINT);
extern BOOL delegate_real_AttachConsole(const char *, const int, const char *, DWORD);
extern BOOL delegate_real_FreeConsole(const char *, const int, const char *);
extern int delegate_real_MultiByteToWideChar(const char *, const int, const char *, UINT, DWORD, LPCCH, int, LPWSTR,
                                             int);
extern int delegate_real_WideCharToMultiByte(const char *, const int, const char *, UINT, DWORD, LPCWCH, int, LPSTR,
                                             int, LPCCH, LPBOOL);
extern void delegate_real_OutputDebugStringW(const char *, const int, const char *, LPCWSTR);
extern BOOL delegate_real_SetHandleInformation(const char *, const int, const char *, HANDLE, DWORD, DWORD);
extern BOOL delegate_real_SetEvent(const char *, const int, const char *, HANDLE);
extern BOOL delegate_real_ResetEvent(const char *, const int, const char *, HANDLE);
extern BOOL delegate_real_GetTokenInformation(const char *, const int, const char *, HANDLE, TOKEN_INFORMATION_CLASS,
                                              LPVOID, DWORD, PDWORD);
extern BOOL delegate_real_ProcessIdToSessionId(const char *, const int, const char *, DWORD, DWORD *);
extern BOOL delegate_real_GetNamedPipeClientProcessId(const char *, const int, const char *, HANDLE, PULONG);
extern DWORD delegate_real_GetTempPathW(const char *, const int, const char *, DWORD, LPWSTR);
extern UINT delegate_real_GetTempFileNameW(const char *, const int, const char *, LPCWSTR, LPCWSTR, UINT, LPWSTR);
extern HMODULE delegate_real_GetModuleHandleW(const char *, const int, const char *, LPCWSTR);
extern FARPROC delegate_real_GetProcAddress(const char *, const int, const char *, HMODULE, LPCSTR);
extern int delegate_real_CompareStringOrdinal(const char *, const int, const char *, LPCWCH, int, LPCWCH, int, BOOL);
extern ULONG delegate_real_StartTraceW(const char *, const int, const char *, CONTROLTRACE_ID *, LPCWSTR,
                                       PEVENT_TRACE_PROPERTIES);
extern ULONG delegate_real_ControlTraceW(const char *, const int, const char *, CONTROLTRACE_ID, LPCWSTR,
                                         PEVENT_TRACE_PROPERTIES, ULONG);
extern ULONG delegate_real_EnableTraceEx2(const char *, const int, const char *, CONTROLTRACE_ID, LPCGUID, ULONG, UCHAR,
                                          ULONGLONG, ULONGLONG, ULONG, PENABLE_TRACE_PARAMETERS);
extern PROCESSTRACE_HANDLE delegate_real_OpenTraceW(const char *, const int, const char *, PEVENT_TRACE_LOGFILEW);
extern ULONG delegate_real_ProcessTrace(const char *, const int, const char *, PROCESSTRACE_HANDLE *, ULONG, LPFILETIME,
                                        LPFILETIME);
extern ULONG delegate_real_CloseTrace(const char *, const int, const char *, PROCESSTRACE_HANDLE);

class Mock_windows
{
  public:
    MOCK_METHOD(ULONGLONG, GetTickCount64, (const char *, const int, const char *));
    MOCK_METHOD(BOOL, QueryPerformanceFrequency, (const char *, const int, const char *, LARGE_INTEGER *));
    MOCK_METHOD(BOOL, QueryPerformanceCounter, (const char *, const int, const char *, LARGE_INTEGER *));
    MOCK_METHOD(void, GetSystemTimeAsFileTime, (const char *, const int, const char *, LPFILETIME));
    MOCK_METHOD(HANDLE, GetStdHandle, (const char *, const int, const char *, DWORD));
    MOCK_METHOD(BOOL, GetConsoleMode, (const char *, const int, const char *, HANDLE, LPDWORD));
    MOCK_METHOD(BOOL, GetConsoleScreenBufferInfo,
                (const char *, const int, const char *, HANDLE, PCONSOLE_SCREEN_BUFFER_INFO));
    MOCK_METHOD(BOOL, SetConsoleMode, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(DWORD, WaitForSingleObject, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(BOOL, ReadConsoleW,
                (const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, PCONSOLE_READCONSOLE_CONTROL));
    MOCK_METHOD(BOOL, WriteConsoleW,
                (const char *, const int, const char *, HANDLE, const VOID *, DWORD, LPDWORD, LPVOID));
    MOCK_METHOD(BOOL, ReadFile, (const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED));
    MOCK_METHOD(BOOL, WriteFile,
                (const char *, const int, const char *, HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED));
    MOCK_METHOD(HANDLE, CreateFileMappingA,
                (const char *, const int, const char *, HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR));
    MOCK_METHOD(LPVOID, MapViewOfFile, (const char *, const int, const char *, HANDLE, DWORD, DWORD, DWORD, SIZE_T));
    MOCK_METHOD(BOOL, UnmapViewOfFile, (const char *, const int, const char *, LPCVOID));
    MOCK_METHOD(BOOL, FlushViewOfFile, (const char *, const int, const char *, LPCVOID, SIZE_T));
    MOCK_METHOD(BOOL, FlushFileBuffers, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(BOOL, GetFileTime, (const char *, const int, const char *, HANDLE, LPFILETIME, LPFILETIME, LPFILETIME));
    MOCK_METHOD(BOOL, SetFileTime,
                (const char *, const int, const char *, HANDLE, const FILETIME *, const FILETIME *, const FILETIME *));
    MOCK_METHOD(BOOL, CloseHandle, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(DWORD, GetLastError, (const char *, const int, const char *));
    MOCK_METHOD(DWORD, GetModuleFileNameW, (const char *, const int, const char *, HMODULE, LPWSTR, DWORD));
    MOCK_METHOD(BOOL, GetComputerNameExW,
                (const char *, const int, const char *, COMPUTER_NAME_FORMAT, LPWSTR, LPDWORD));
    MOCK_METHOD(BOOL, GetUserPreferredUILanguages,
                (const char *, const int, const char *, DWORD, PULONG, PZZWSTR, PULONG));
    MOCK_METHOD(int, GetUserDefaultLocaleName, (const char *, const int, const char *, LPWSTR, int));
    MOCK_METHOD(BOOL, GetExitCodeProcess, (const char *, const int, const char *, HANDLE, LPDWORD));
    MOCK_METHOD(BOOL, TerminateProcess, (const char *, const int, const char *, HANDLE, UINT));
    MOCK_METHOD(BOOL, InitializeProcThreadAttributeList,
                (const char *, const int, const char *, LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T));
    MOCK_METHOD(BOOL, UpdateProcThreadAttribute,
                (const char *, const int, const char *, LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T,
                 PVOID, PSIZE_T));
    MOCK_METHOD(void, DeleteProcThreadAttributeList,
                (const char *, const int, const char *, LPPROC_THREAD_ATTRIBUTE_LIST));
    MOCK_METHOD(BOOL, CreateProcessW,
                (const char *, const int, const char *, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
                 BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION));
    MOCK_METHOD(HANDLE, GetCurrentProcess, (const char *, const int, const char *));
    MOCK_METHOD(DWORD, GetCurrentProcessId, (const char *, const int, const char *));
    MOCK_METHOD(BOOL, DuplicateHandle,
                (const char *, const int, const char *, HANDLE, HANDLE, HANDLE, LPHANDLE, DWORD, BOOL, DWORD));
    MOCK_METHOD(HANDLE, OpenProcess, (const char *, const int, const char *, DWORD, BOOL, DWORD));
    MOCK_METHOD(HANDLE, OpenThread, (const char *, const int, const char *, DWORD, BOOL, DWORD));
    MOCK_METHOD(BOOL, SetPriorityClass, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(DWORD, GetPriorityClass, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(BOOL, SetProcessInformation,
                (const char *, const int, const char *, HANDLE, PROCESS_INFORMATION_CLASS, LPVOID, DWORD));
    MOCK_METHOD(BOOL, GetProcessInformation,
                (const char *, const int, const char *, HANDLE, PROCESS_INFORMATION_CLASS, LPVOID, DWORD));
    MOCK_METHOD(BOOL, SetProcessPriorityBoost, (const char *, const int, const char *, HANDLE, BOOL));
    MOCK_METHOD(BOOL, GetProcessPriorityBoost, (const char *, const int, const char *, HANDLE, PBOOL));
    MOCK_METHOD(BOOL, SetThreadPriority, (const char *, const int, const char *, HANDLE, int));
    MOCK_METHOD(int, GetThreadPriority, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(BOOL, SetStdHandle, (const char *, const int, const char *, DWORD, HANDLE));
    MOCK_METHOD(DWORD, GetFileType, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(HANDLE, CreateFileW,
                (const char *, const int, const char *, LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD,
                 HANDLE));
    MOCK_METHOD(BOOL, SetServiceStatus,
                (const char *, const int, const char *, SERVICE_STATUS_HANDLE, LPSERVICE_STATUS));
    MOCK_METHOD(BOOL, SetConsoleCtrlHandler, (const char *, const int, const char *, PHANDLER_ROUTINE, BOOL));
    MOCK_METHOD(UINT, GetConsoleCP, (const char *, const int, const char *));
    MOCK_METHOD(UINT, GetConsoleOutputCP, (const char *, const int, const char *));
    MOCK_METHOD(BOOL, SetConsoleCP, (const char *, const int, const char *, UINT));
    MOCK_METHOD(BOOL, SetConsoleOutputCP, (const char *, const int, const char *, UINT));
    MOCK_METHOD(BOOL, AttachConsole, (const char *, const int, const char *, DWORD));
    MOCK_METHOD(BOOL, FreeConsole, (const char *, const int, const char *));
    MOCK_METHOD(int, MultiByteToWideChar,
                (const char *, const int, const char *, UINT, DWORD, LPCCH, int, LPWSTR, int));
    MOCK_METHOD(int, WideCharToMultiByte,
                (const char *, const int, const char *, UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL));
    MOCK_METHOD(void, OutputDebugStringW, (const char *, const int, const char *, LPCWSTR));
    MOCK_METHOD(BOOL, SetHandleInformation, (const char *, const int, const char *, HANDLE, DWORD, DWORD));
    MOCK_METHOD(BOOL, SetEvent, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(BOOL, ResetEvent, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(BOOL, GetTokenInformation,
                (const char *, const int, const char *, HANDLE, TOKEN_INFORMATION_CLASS, LPVOID, DWORD, PDWORD));
    MOCK_METHOD(BOOL, ProcessIdToSessionId, (const char *, const int, const char *, DWORD, DWORD *));
    MOCK_METHOD(BOOL, GetNamedPipeClientProcessId, (const char *, const int, const char *, HANDLE, PULONG));
    MOCK_METHOD(DWORD, GetTempPathW, (const char *, const int, const char *, DWORD, LPWSTR));
    MOCK_METHOD(UINT, GetTempFileNameW,
                (const char *, const int, const char *, LPCWSTR, LPCWSTR, UINT, LPWSTR));
    MOCK_METHOD(HMODULE, GetModuleHandleW, (const char *, const int, const char *, LPCWSTR));
    MOCK_METHOD(FARPROC, GetProcAddress, (const char *, const int, const char *, HMODULE, LPCSTR));
    MOCK_METHOD(int, CompareStringOrdinal,
                (const char *, const int, const char *, LPCWCH, int, LPCWCH, int, BOOL));
    MOCK_METHOD(ULONG, StartTraceW,
                (const char *, const int, const char *, CONTROLTRACE_ID *, LPCWSTR, PEVENT_TRACE_PROPERTIES));
    MOCK_METHOD(ULONG, ControlTraceW,
                (const char *, const int, const char *, CONTROLTRACE_ID, LPCWSTR, PEVENT_TRACE_PROPERTIES, ULONG));
    MOCK_METHOD(ULONG, EnableTraceEx2,
                (const char *, const int, const char *, CONTROLTRACE_ID, LPCGUID, ULONG, UCHAR, ULONGLONG, ULONGLONG,
                 ULONG, PENABLE_TRACE_PARAMETERS));
    MOCK_METHOD(PROCESSTRACE_HANDLE, OpenTraceW,
                (const char *, const int, const char *, PEVENT_TRACE_LOGFILEW));
    MOCK_METHOD(ULONG, ProcessTrace,
                (const char *, const int, const char *, PROCESSTRACE_HANDLE *, ULONG, LPFILETIME, LPFILETIME));
    MOCK_METHOD(ULONG, CloseTrace, (const char *, const int, const char *, PROCESSTRACE_HANDLE));

    void switch_to_real_time();
    void switch_to_mock_time();

    Mock_windows();
    virtual ~Mock_windows();
};

extern Mock_windows *_mock_windows;

        #endif // _IN_OVERRIDE_HEADER_WINDOWS_H

    #endif // _MOCK_WINDOWS_H

#endif // _WIN32
