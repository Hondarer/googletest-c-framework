#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

ULONG delegate_real_ProcessTrace(const char *file, const int line, const char *func, PROCESSTRACE_HANDLE *handle_array,
                                 ULONG handle_count, LPFILETIME start_time, LPFILETIME end_time)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ProcessTrace(handle_array, handle_count, start_time, end_time);
}

ULONG mock_ProcessTrace(const char *file, const int line, const char *func, PROCESSTRACE_HANDLE *handle_array,
                        ULONG handle_count, LPFILETIME start_time, LPFILETIME end_time)
{
    ULONG mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->ProcessTrace(file, line, func, handle_array, handle_count, start_time, end_time);
    }
    else
    {
        mock_ret = delegate_real_ProcessTrace(file, line, func, handle_array, handle_count, start_time, end_time);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ProcessTrace 0x%p, %lu", (void *)handle_array, (unsigned long)handle_count);
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
