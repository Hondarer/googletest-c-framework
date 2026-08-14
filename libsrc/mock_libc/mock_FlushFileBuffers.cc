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
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->FlushFileBuffers(file, line, func, mapping_file);
    }
    else
    {
        mock_ret = delegate_real_FlushFileBuffers(file, line, func, mapping_file);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > FlushFileBuffers 0x%p", (void *)mapping_file);
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
