#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

LPVOID delegate_real_MapViewOfFile(const char *file, const int line, const char *func, HANDLE mapping, DWORD access,
                                   DWORD offset_high, DWORD offset_low, SIZE_T bytes)
{
    (void)file;
    (void)line;
    (void)func;

    return MapViewOfFile(mapping, access, offset_high, offset_low, bytes);
}

LPVOID mock_MapViewOfFile(const char *file, const int line, const char *func, HANDLE mapping, DWORD access,
                          DWORD offset_high, DWORD offset_low, SIZE_T bytes)
{
    LPVOID rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->MapViewOfFile(file, line, func, mapping, access, offset_high, offset_low, bytes);
    }
    else
    {
        rtc = delegate_real_MapViewOfFile(file, line, func, mapping, access, offset_high, offset_low, bytes);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > MapViewOfFile 0x%p, %lu, %lu, %lu, %llu", (void *)mapping, access, offset_high, offset_low,
               (unsigned long long)bytes);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
