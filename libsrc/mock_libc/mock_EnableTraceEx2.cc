#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

ULONG delegate_real_EnableTraceEx2(const char *file, const int line, const char *func, CONTROLTRACE_ID trace_id,
                                   LPCGUID provider_id, ULONG control_code, UCHAR level, ULONGLONG match_any_keyword,
                                   ULONGLONG match_all_keyword, ULONG timeout,
                                   PENABLE_TRACE_PARAMETERS enable_parameters)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return EnableTraceEx2(trace_id, provider_id, control_code, level, match_any_keyword, match_all_keyword, timeout,
                          enable_parameters);
}

ULONG mock_EnableTraceEx2(const char *file, const int line, const char *func, CONTROLTRACE_ID trace_id,
                          LPCGUID provider_id, ULONG control_code, UCHAR level, ULONGLONG match_any_keyword,
                          ULONGLONG match_all_keyword, ULONG timeout, PENABLE_TRACE_PARAMETERS enable_parameters)
{
    ULONG mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->EnableTraceEx2(file, line, func, trace_id, provider_id, control_code, level,
                                                 match_any_keyword, match_all_keyword, timeout, enable_parameters);
    }
    else
    {
        mock_ret = delegate_real_EnableTraceEx2(file, line, func, trace_id, provider_id, control_code, level,
                                                match_any_keyword, match_all_keyword, timeout, enable_parameters);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EnableTraceEx2 %llu, 0x%p, %lu, %u", (unsigned long long)trace_id, (const void *)provider_id,
               (unsigned long)control_code, (unsigned int)level);
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
