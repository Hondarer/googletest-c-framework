#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_ProcessIdToSessionId(const char *file, const int line, const char *func, DWORD process_id,
                                        DWORD *session_id)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ProcessIdToSessionId(process_id, session_id);
}

BOOL mock_ProcessIdToSessionId(const char *file, const int line, const char *func, DWORD process_id, DWORD *session_id)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->ProcessIdToSessionId(file, line, func, process_id, session_id);
    }
    else
    {
        mock_ret = delegate_real_ProcessIdToSessionId(file, line, func, process_id, session_id);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ProcessIdToSessionId %lu", process_id);
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
