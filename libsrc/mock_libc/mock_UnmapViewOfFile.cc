#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_UnmapViewOfFile(const char *file, const int line, const char *func, LPCVOID address)
{
    (void)file;
    (void)line;
    (void)func;

    return UnmapViewOfFile(address);
}

BOOL mock_UnmapViewOfFile(const char *file, const int line, const char *func, LPCVOID address)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->UnmapViewOfFile(file, line, func, address);
    }
    else
    {
        rtc = delegate_real_UnmapViewOfFile(file, line, func, address);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > UnmapViewOfFile 0x%p", address);
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
