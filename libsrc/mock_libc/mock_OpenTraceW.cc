#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

PROCESSTRACE_HANDLE delegate_real_OpenTraceW(const char *file, const int line, const char *func,
                                           PEVENT_TRACE_LOGFILEW logfile)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return OpenTraceW(logfile);
}

PROCESSTRACE_HANDLE mock_OpenTraceW(const char *file, const int line, const char *func, PEVENT_TRACE_LOGFILEW logfile)
{
    PROCESSTRACE_HANDLE mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->OpenTraceW(file, line, func, logfile);
    }
    else
    {
        mock_ret = delegate_real_OpenTraceW(file, line, func, logfile);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > OpenTraceW 0x%p", (void *)logfile);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %llu\n", file, line, (unsigned long long)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
