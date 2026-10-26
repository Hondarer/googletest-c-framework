#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetThreadPriority(const char *file, const int line, const char *func, HANDLE thread, int priority)
{
    (void)file;
    (void)line;
    (void)func;

    return SetThreadPriority(thread, priority);
}

BOOL mock_SetThreadPriority(const char *file, const int line, const char *func, HANDLE thread, int priority)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetThreadPriority(file, line, func, thread, priority);
    }
    else
    {
        mock_ret = delegate_real_SetThreadPriority(file, line, func, thread, priority);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetThreadPriority");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, (int)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
