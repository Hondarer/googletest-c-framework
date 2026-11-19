#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetConsoleCtrlHandler(const char *file, const int line, const char *func,
                                         PHANDLER_ROUTINE handler_routine, BOOL add)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return SetConsoleCtrlHandler(handler_routine, add);
}

BOOL mock_SetConsoleCtrlHandler(const char *file, const int line, const char *func, PHANDLER_ROUTINE handler_routine,
                                BOOL add)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetConsoleCtrlHandler(file, line, func, handler_routine, add);
    }
    else
    {
        mock_ret = delegate_real_SetConsoleCtrlHandler(file, line, func, handler_routine, add);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetConsoleCtrlHandler 0x%p, %d", (void *)handler_routine, add);
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
