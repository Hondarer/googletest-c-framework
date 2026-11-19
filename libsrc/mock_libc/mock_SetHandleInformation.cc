#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetHandleInformation(const char *file, const int line, const char *func, HANDLE object, DWORD mask,
                                        DWORD flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return SetHandleInformation(object, mask, flags);
}

BOOL mock_SetHandleInformation(const char *file, const int line, const char *func, HANDLE object, DWORD mask,
                               DWORD flags)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetHandleInformation(file, line, func, object, mask, flags);
    }
    else
    {
        mock_ret = delegate_real_SetHandleInformation(file, line, func, object, mask, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetHandleInformation 0x%p, 0x%lx, 0x%lx", (void *)object, mask, flags);
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
