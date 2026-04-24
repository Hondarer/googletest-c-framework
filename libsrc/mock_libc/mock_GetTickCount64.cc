#ifdef _WIN32

#include <mock_windows.h>
#include <test_com.h>

using namespace testing;

ULONGLONG delegate_fake_GetTickCount64(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return 0;
}

ULONGLONG delegate_real_GetTickCount64(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetTickCount64();
}

ULONGLONG mock_GetTickCount64(const char *file, const int line, const char *func)
{
    ULONGLONG rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->GetTickCount64(file, line, func);
    }
    else
    {
        rtc = delegate_real_GetTickCount64(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetTickCount64");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %llu\n", file, line, (unsigned long long)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
