#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetProcessPriorityBoost(const char *file, const int line, const char *func, HANDLE process, PBOOL disable_priority_boost)
{
    (void)file;
    (void)line;
    (void)func;

    return GetProcessPriorityBoost(process, disable_priority_boost);
}

BOOL mock_GetProcessPriorityBoost(const char *file, const int line, const char *func, HANDLE process, PBOOL disable_priority_boost)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetProcessPriorityBoost(file, line, func, process, disable_priority_boost);
    }
    else
    {
        mock_ret = delegate_real_GetProcessPriorityBoost(file, line, func, process, disable_priority_boost);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetProcessPriorityBoost");
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
