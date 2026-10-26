#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

DWORD delegate_real_GetPriorityClass(const char *file, const int line, const char *func, HANDLE process)
{
    (void)file;
    (void)line;
    (void)func;

    return GetPriorityClass(process);
}

DWORD mock_GetPriorityClass(const char *file, const int line, const char *func, HANDLE process)
{
    DWORD mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetPriorityClass(file, line, func, process);
    }
    else
    {
        mock_ret = delegate_real_GetPriorityClass(file, line, func, process);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetPriorityClass");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%lx\n", file, line, (unsigned long)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
