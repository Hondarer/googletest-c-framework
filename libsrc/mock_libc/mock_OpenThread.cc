#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

HANDLE delegate_real_OpenThread(const char *file, const int line, const char *func, DWORD desired_access, BOOL inherit_handle, DWORD thread_id)
{
    (void)file;
    (void)line;
    (void)func;

    return OpenThread(desired_access, inherit_handle, thread_id);
}

HANDLE mock_OpenThread(const char *file, const int line, const char *func, DWORD desired_access, BOOL inherit_handle, DWORD thread_id)
{
    HANDLE mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->OpenThread(file, line, func, desired_access, inherit_handle, thread_id);
    }
    else
    {
        mock_ret = delegate_real_OpenThread(file, line, func, desired_access, inherit_handle, thread_id);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > OpenThread");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
