#ifdef _WIN32

    #ifndef _MOCK_WINDOWS_H
        #define _MOCK_WINDOWS_H

        // In C++17 mode, std::byte conflicts with the Windows SDK's typedef unsigned char byte;
        // in rpcndr.h. Setting _HAS_STD_BYTE to 0 suppresses the conflicting using-declaration
        // in the Windows SDK headers.
        #ifndef _HAS_STD_BYTE
            #define _HAS_STD_BYTE 0
        #endif

        #include <windows.h>

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern ULONGLONG mock_GetTickCount64(const char *, const int, const char *);
    extern void mock_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
    extern HANDLE mock_GetStdHandle(const char *, const int, const char *, DWORD);
    extern BOOL mock_GetConsoleMode(const char *, const int, const char *, HANDLE, LPDWORD);
    extern BOOL mock_SetConsoleMode(const char *, const int, const char *, HANDLE, DWORD);
    extern DWORD mock_WaitForSingleObject(const char *, const int, const char *, HANDLE, DWORD);
    extern BOOL mock_ReadFile(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);

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
            #define SetConsoleMode(console_handle, mode) \
                mock_SetConsoleMode(__FILE__, __LINE__, __func__, console_handle, mode)
            #define WaitForSingleObject(handle, milliseconds) \
                mock_WaitForSingleObject(__FILE__, __LINE__, __func__, handle, milliseconds)
            #define ReadFile(file_handle, buffer, bytes_to_read, bytes_read, overlapped) \
                mock_ReadFile(__FILE__, __LINE__, __func__, file_handle, buffer, bytes_to_read, bytes_read, overlapped)

        #else // _IN_OVERRIDE_HEADER_WINDOWS_H

            #include <gmock/gmock.h>

extern ULONGLONG delegate_real_GetTickCount64(const char *, const int, const char *);
extern ULONGLONG delegate_fake_GetTickCount64(const char *, const int, const char *);
extern void delegate_real_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern void delegate_fake_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern HANDLE delegate_real_GetStdHandle(const char *, const int, const char *, DWORD);
extern BOOL delegate_real_GetConsoleMode(const char *, const int, const char *, HANDLE, LPDWORD);
extern BOOL delegate_real_SetConsoleMode(const char *, const int, const char *, HANDLE, DWORD);
extern DWORD delegate_real_WaitForSingleObject(const char *, const int, const char *, HANDLE, DWORD);
extern BOOL delegate_real_ReadFile(const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD,
                                   LPOVERLAPPED);

class Mock_windows
{
  public:
    MOCK_METHOD(ULONGLONG, GetTickCount64, (const char *, const int, const char *));
    MOCK_METHOD(void, GetSystemTimeAsFileTime, (const char *, const int, const char *, LPFILETIME));
    MOCK_METHOD(HANDLE, GetStdHandle, (const char *, const int, const char *, DWORD));
    MOCK_METHOD(BOOL, GetConsoleMode, (const char *, const int, const char *, HANDLE, LPDWORD));
    MOCK_METHOD(BOOL, SetConsoleMode, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(DWORD, WaitForSingleObject, (const char *, const int, const char *, HANDLE, DWORD));
    MOCK_METHOD(BOOL, ReadFile, (const char *, const int, const char *, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED));

    void switch_to_real_time();
    void switch_to_mock_time();

    Mock_windows();
    ~Mock_windows();
};

extern Mock_windows *_mock_windows;

        #endif // _IN_OVERRIDE_HEADER_WINDOWS_H

    #endif // _MOCK_WINDOWS_H

#endif // _WIN32
