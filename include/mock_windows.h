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
        #ifdef byte
            #undef byte /* C++17 std::byte と Windows SDK byte typedef の競合を解消 */
        #endif          /* byte */

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern ULONGLONG mock_GetTickCount64(const char *, const int, const char *);
    extern void mock_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
    extern HANDLE mock_GetStdHandle(const char *, const int, const char *, DWORD);
    extern BOOL mock_GetConsoleMode(const char *, const int, const char *, HANDLE, LPDWORD);
    extern BOOL mock_GetConsoleScreenBufferInfo(const char *, const int, const char *, HANDLE,
                                                PCONSOLE_SCREEN_BUFFER_INFO);
    extern BOOL mock_SetConsoleMode(const char *, const int, const char *, HANDLE, DWORD);
    extern DWORD mock_WaitForSingleObject(const char *, const int, const char *, HANDLE, DWORD);
    extern BOOL mock_ReadFile(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
    extern HANDLE mock_CreateFileMappingA(const char *, const int, const char *, HANDLE, LPSECURITY_ATTRIBUTES, DWORD,
                                          DWORD, DWORD, LPCSTR);
    extern LPVOID mock_MapViewOfFile(const char *, const int, const char *, HANDLE, DWORD, DWORD, DWORD, SIZE_T);
    extern BOOL mock_UnmapViewOfFile(const char *, const int, const char *, LPCVOID);
    extern BOOL mock_FlushViewOfFile(const char *, const int, const char *, LPCVOID, SIZE_T);
    extern BOOL mock_FlushFileBuffers(const char *, const int, const char *, HANDLE);
    extern BOOL mock_CloseHandle(const char *, const int, const char *, HANDLE);
    extern DWORD mock_GetLastError(const char *, const int, const char *);
    extern DWORD mock_GetModuleFileNameW(const char *, const int, const char *, HMODULE, LPWSTR, DWORD);
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

        #ifdef __cplusplus
}
        #endif

        #ifdef _IN_OVERRIDE_HEADER_WINDOWS_H

            #define GetTickCount64() mock_GetTickCount64(__FILE__, __LINE__, __func__)
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
            #define ReadFile(file_handle, buffer, bytes_to_read, bytes_read, overlapped) \
                mock_ReadFile(__FILE__, __LINE__, __func__, file_handle, buffer, bytes_to_read, bytes_read, overlapped)
            #define CreateFileMappingA(file, attributes, protect, size_high, size_low, name) \
                mock_CreateFileMappingA(__FILE__, __LINE__, __func__, file, attributes, protect, size_high, size_low, \
                                        name)
            #define MapViewOfFile(mapping, access, offset_high, offset_low, bytes) \
                mock_MapViewOfFile(__FILE__, __LINE__, __func__, mapping, access, offset_high, offset_low, bytes)
            #define UnmapViewOfFile(address)        mock_UnmapViewOfFile(__FILE__, __LINE__, __func__, address)
            #define FlushViewOfFile(address, bytes) mock_FlushViewOfFile(__FILE__, __LINE__, __func__, address, bytes)
            #define FlushFileBuffers(file)          mock_FlushFileBuffers(__FILE__, __LINE__, __func__, file)
            #define CloseHandle(handle)             mock_CloseHandle(__FILE__, __LINE__, __func__, handle)
            #define GetLastError()                  mock_GetLastError(__FILE__, __LINE__, __func__)
            #define GetModuleFileNameW(module, filename, size) \
                mock_GetModuleFileNameW(__FILE__, __LINE__, __func__, module, filename, size)
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
            #define GetCurrentProcess() mock_GetCurrentProcess(__FILE__, __LINE__, __func__)
            #define GetCurrentProcessId() mock_GetCurrentProcessId(__FILE__, __LINE__, __func__)
            #define DuplicateHandle(source_process, source_handle, target_process, target_handle, desired_access, \
                                    inherit_handle, options) \
                mock_DuplicateHandle(__FILE__, __LINE__, __func__, source_process, source_handle, target_process, \
                                     target_handle, desired_access, inherit_handle, options)

        #else // _IN_OVERRIDE_HEADER_WINDOWS_H

            #include <gmock/gmock.h>

extern ULONGLONG delegate_real_GetTickCount64(const char *, const int, const char *);
extern ULONGLONG delegate_fake_GetTickCount64(const char *, const int, const char *);
extern void delegate_real_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern void delegate_fake_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern HANDLE delegate_real_GetStdHandle(const char *, const int, const char *, DWORD);
extern BOOL delegate_real_GetConsoleMode(const char *, const int, const char *, HANDLE, LPDWORD);
extern BOOL delegate_real_GetConsoleScreenBufferInfo(const char *, const int, const char *, HANDLE,
                                                     PCONSOLE_SCREEN_BUFFER_INFO);
extern BOOL delegate_real_SetConsoleMode(const char *, const int, const char *, HANDLE, DWORD);
extern DWORD delegate_real_WaitForSingleObject(const char *, const int, const char *, HANDLE, DWORD);
extern BOOL delegate_real_ReadFile(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
extern HANDLE delegate_real_CreateFileMappingA(const char *, const int, const char *, HANDLE, LPSECURITY_ATTRIBUTES,
                                               DWORD, DWORD, DWORD, LPCSTR);
extern LPVOID delegate_real_MapViewOfFile(const char *, const int, const char *, HANDLE, DWORD, DWORD, DWORD, SIZE_T);
extern BOOL delegate_real_UnmapViewOfFile(const char *, const int, const char *, LPCVOID);
extern BOOL delegate_real_FlushViewOfFile(const char *, const int, const char *, LPCVOID, SIZE_T);
extern BOOL delegate_real_FlushFileBuffers(const char *, const int, const char *, HANDLE);
extern BOOL delegate_real_CloseHandle(const char *, const int, const char *, HANDLE);
extern DWORD delegate_real_GetLastError(const char *, const int, const char *);
extern DWORD delegate_real_GetModuleFileNameW(const char *, const int, const char *, HMODULE, LPWSTR, DWORD);
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

class Mock_windows
{
  public:
    MOCK_METHOD(ULONGLONG, GetTickCount64, (const char *, const int, const char *));
    MOCK_METHOD(void, GetSystemTimeAsFileTime, (const char *, const int, const char *, LPFILETIME));
    MOCK_METHOD(HANDLE, GetStdHandle, (const char *, const int, const char *, DWORD));
    MOCK_METHOD(BOOL, GetConsoleMode, (const char *, const int, const char *, HANDLE, LPDWORD));
    MOCK_METHOD(BOOL, GetConsoleScreenBufferInfo,
                (const char *, const int, const char *, HANDLE, PCONSOLE_SCREEN_BUFFER_INFO));
    MOCK_METHOD(BOOL, SetConsoleMode, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(DWORD, WaitForSingleObject, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(BOOL, ReadFile, (const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED));
    MOCK_METHOD(HANDLE, CreateFileMappingA,
                (const char *, const int, const char *, HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR));
    MOCK_METHOD(LPVOID, MapViewOfFile, (const char *, const int, const char *, HANDLE, DWORD, DWORD, DWORD, SIZE_T));
    MOCK_METHOD(BOOL, UnmapViewOfFile, (const char *, const int, const char *, LPCVOID));
    MOCK_METHOD(BOOL, FlushViewOfFile, (const char *, const int, const char *, LPCVOID, SIZE_T));
    MOCK_METHOD(BOOL, FlushFileBuffers, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(BOOL, CloseHandle, (const char *, const int, const char *, HANDLE));
    MOCK_METHOD(DWORD, GetLastError, (const char *, const int, const char *));
    MOCK_METHOD(DWORD, GetModuleFileNameW, (const char *, const int, const char *, HMODULE, LPWSTR, DWORD));
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

    void switch_to_real_time();
    void switch_to_mock_time();

    Mock_windows();
    ~Mock_windows();
};

extern Mock_windows *_mock_windows;

        #endif // _IN_OVERRIDE_HEADER_WINDOWS_H

    #endif // _MOCK_WINDOWS_H

#endif // _WIN32
