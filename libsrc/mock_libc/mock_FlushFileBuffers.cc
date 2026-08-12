#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_FlushFileBuffers(const char *file, const int line, const char *func, HANDLE mapping_file)
{
    (void)file;
    (void)line;
    (void)func;

    return FlushFileBuffers(mapping_file);
}

BOOL mock_FlushFileBuffers(const char *file, const int line, const char *func, HANDLE mapping_file)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->FlushFileBuffers(file, line, func, mapping_file);
    }
    else
    {
        rtc = delegate_real_FlushFileBuffers(file, line, func, mapping_file);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > FlushFileBuffers 0x%p", (void *)mapping_file);
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
