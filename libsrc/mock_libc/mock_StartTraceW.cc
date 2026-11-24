#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

ULONG delegate_real_StartTraceW(const char *file, const int line, const char *func, CONTROLTRACE_ID *trace_id,
                                LPCWSTR instance_name, PEVENT_TRACE_PROPERTIES properties)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return StartTraceW(trace_id, instance_name, properties);
}

ULONG mock_StartTraceW(const char *file, const int line, const char *func, CONTROLTRACE_ID *trace_id,
                       LPCWSTR instance_name, PEVENT_TRACE_PROPERTIES properties)
{
    ULONG mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->StartTraceW(file, line, func, trace_id, instance_name, properties);
    }
    else
    {
        mock_ret = delegate_real_StartTraceW(file, line, func, trace_id, instance_name, properties);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > StartTraceW 0x%p, 0x%p, 0x%p", (void *)trace_id, (const void *)instance_name, (void *)properties);
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
