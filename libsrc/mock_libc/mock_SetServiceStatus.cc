#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

BOOL delegate_real_SetServiceStatus(const char *file, const int line, const char *func,
                                    SERVICE_STATUS_HANDLE service_status_handle, LPSERVICE_STATUS service_status)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return SetServiceStatus(service_status_handle, service_status);
}

BOOL mock_SetServiceStatus(const char *file, const int line, const char *func,
                           SERVICE_STATUS_HANDLE service_status_handle, LPSERVICE_STATUS service_status)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetServiceStatus(file, line, func, service_status_handle, service_status);
    }
    else
    {
        mock_ret = delegate_real_SetServiceStatus(file, line, func, service_status_handle, service_status);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetServiceStatus 0x%p", (void *)service_status_handle);
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
