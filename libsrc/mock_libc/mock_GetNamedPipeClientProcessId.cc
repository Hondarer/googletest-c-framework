#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetNamedPipeClientProcessId(const char *file, const int line, const char *func, HANDLE pipe,
                                               PULONG client_process_id)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetNamedPipeClientProcessId(pipe, client_process_id);
}

BOOL mock_GetNamedPipeClientProcessId(const char *file, const int line, const char *func, HANDLE pipe,
                                      PULONG client_process_id)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetNamedPipeClientProcessId(file, line, func, pipe, client_process_id);
    }
    else
    {
        mock_ret = delegate_real_GetNamedPipeClientProcessId(file, line, func, pipe, client_process_id);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetNamedPipeClientProcessId 0x%p", (void *)pipe);
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
