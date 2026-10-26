#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetPriorityClass(const char *file, const int line, const char *func, HANDLE process, DWORD priority_class)
{
    (void)file;
    (void)line;
    (void)func;

    return SetPriorityClass(process, priority_class);
}

BOOL mock_SetPriorityClass(const char *file, const int line, const char *func, HANDLE process, DWORD priority_class)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetPriorityClass(file, line, func, process, priority_class);
    }
    else
    {
        mock_ret = delegate_real_SetPriorityClass(file, line, func, process, priority_class);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetPriorityClass");
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
