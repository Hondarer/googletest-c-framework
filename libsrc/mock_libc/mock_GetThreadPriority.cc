#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

int delegate_real_GetThreadPriority(const char *file, const int line, const char *func, HANDLE thread)
{
    (void)file;
    (void)line;
    (void)func;

    return GetThreadPriority(thread);
}

int mock_GetThreadPriority(const char *file, const int line, const char *func, HANDLE thread)
{
    int mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetThreadPriority(file, line, func, thread);
    }
    else
    {
        mock_ret = delegate_real_GetThreadPriority(file, line, func, thread);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetThreadPriority");
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
