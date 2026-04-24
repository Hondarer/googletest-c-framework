#ifdef _WIN32

#ifndef _MOCK_WINDOWS_H
#define _MOCK_WINDOWS_H

#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern ULONGLONG mock_GetTickCount64(const char *, const int, const char *);
    extern void mock_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_WINDOWS_H

#define GetTickCount64() mock_GetTickCount64(__FILE__, __LINE__, __func__)
#define GetSystemTimeAsFileTime(file_time) mock_GetSystemTimeAsFileTime(__FILE__, __LINE__, __func__, file_time)

#else // _IN_OVERRIDE_HEADER_WINDOWS_H

#include <gmock/gmock.h>

extern ULONGLONG delegate_real_GetTickCount64(const char *, const int, const char *);
extern ULONGLONG delegate_fake_GetTickCount64(const char *, const int, const char *);
extern void delegate_real_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);
extern void delegate_fake_GetSystemTimeAsFileTime(const char *, const int, const char *, LPFILETIME);

class Mock_windows
{
public:
    MOCK_METHOD(ULONGLONG, GetTickCount64, (const char *, const int, const char *));
    MOCK_METHOD(void, GetSystemTimeAsFileTime, (const char *, const int, const char *, LPFILETIME));

    void switch_to_real_time();
    void switch_to_mock_time();

    Mock_windows();
    ~Mock_windows();
};

extern Mock_windows *_mock_windows;

#endif // _IN_OVERRIDE_HEADER_WINDOWS_H

#endif // _MOCK_WINDOWS_H

#endif // _WIN32
