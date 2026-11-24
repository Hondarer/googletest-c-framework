#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

ULONG delegate_real_ControlTraceW(const char *file, const int line, const char *func, CONTROLTRACE_ID trace_id,
                                  LPCWSTR instance_name, PEVENT_TRACE_PROPERTIES properties, ULONG control_code)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ControlTraceW(trace_id, instance_name, properties, control_code);
}

ULONG mock_ControlTraceW(const char *file, const int line, const char *func, CONTROLTRACE_ID trace_id,
                         LPCWSTR instance_name, PEVENT_TRACE_PROPERTIES properties, ULONG control_code)
{
    ULONG mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->ControlTraceW(file, line, func, trace_id, instance_name, properties, control_code);
    }
    else
    {
        mock_ret = delegate_real_ControlTraceW(file, line, func, trace_id, instance_name, properties, control_code);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ControlTraceW %llu, 0x%p, 0x%p, %lu", (unsigned long long)trace_id, (const void *)instance_name,
               (void *)properties, (unsigned long)control_code);
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
