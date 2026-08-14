#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_DuplicateHandle(const char *file, const int line, const char *func, HANDLE source_process,
                                   HANDLE source_handle, HANDLE target_process, LPHANDLE target_handle,
                                   DWORD desired_access, BOOL inherit_handle, DWORD options)
{
    (void)file;
    (void)line;
    (void)func;

    return DuplicateHandle(source_process, source_handle, target_process, target_handle, desired_access, inherit_handle,
                           options);
}

BOOL mock_DuplicateHandle(const char *file, const int line, const char *func, HANDLE source_process,
                          HANDLE source_handle, HANDLE target_process, LPHANDLE target_handle, DWORD desired_access,
                          BOOL inherit_handle, DWORD options)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->DuplicateHandle(file, line, func, source_process, source_handle, target_process,
                                             target_handle, desired_access, inherit_handle, options);
    }
    else
    {
        rtc = delegate_real_DuplicateHandle(file, line, func, source_process, source_handle, target_process,
                                            target_handle, desired_access, inherit_handle, options);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > DuplicateHandle 0x%p, 0x%p, 0x%p, 0x%p, %lu, %d, %lu", (void *)source_process,
               (void *)source_handle, (void *)target_process, (void *)target_handle, desired_access, inherit_handle,
               options);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
