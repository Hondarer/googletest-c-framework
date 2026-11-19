#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_ResetEvent(const char *file, const int line, const char *func, HANDLE event)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return ResetEvent(event);
}

BOOL mock_ResetEvent(const char *file, const int line, const char *func, HANDLE event)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->ResetEvent(file, line, func, event);
    }
    else
    {
        mock_ret = delegate_real_ResetEvent(file, line, func, event);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ResetEvent 0x%p", (void *)event);
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
