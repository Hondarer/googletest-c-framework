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
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->UnmapViewOfFile(file, line, func, address);
    }
    else
    {
        mock_ret = delegate_real_UnmapViewOfFile(file, line, func, address);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > UnmapViewOfFile 0x%p", address);
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
