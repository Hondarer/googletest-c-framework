#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_FlushViewOfFile(const char *file, const int line, const char *func, LPCVOID address, SIZE_T bytes)
{
    (void)file;
    (void)line;
    (void)func;

    return FlushViewOfFile(address, bytes);
}

BOOL mock_FlushViewOfFile(const char *file, const int line, const char *func, LPCVOID address, SIZE_T bytes)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->FlushViewOfFile(file, line, func, address, bytes);
    }
    else
    {
        mock_ret = delegate_real_FlushViewOfFile(file, line, func, address, bytes);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > FlushViewOfFile 0x%p, %llu", address, (unsigned long long)bytes);
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
