#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

ULONG delegate_real_CloseTrace(const char *file, const int line, const char *func, PROCESSTRACE_HANDLE trace_handle)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return CloseTrace(trace_handle);
}

ULONG mock_CloseTrace(const char *file, const int line, const char *func, PROCESSTRACE_HANDLE trace_handle)
{
    ULONG mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->CloseTrace(file, line, func, trace_handle);
    }
    else
    {
        mock_ret = delegate_real_CloseTrace(file, line, func, trace_handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CloseTrace %llu", (unsigned long long)trace_handle);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lu\n", file, line, (unsigned long)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
