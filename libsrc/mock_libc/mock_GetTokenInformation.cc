#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

    #pragma comment(lib, "Advapi32.lib")

using namespace testing;

BOOL delegate_real_GetTokenInformation(const char *file, const int line, const char *func, HANDLE token_handle,
                                       TOKEN_INFORMATION_CLASS token_information_class, LPVOID token_information,
                                       DWORD token_information_length, PDWORD return_length)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetTokenInformation(token_handle, token_information_class, token_information, token_information_length,
                               return_length);
}

BOOL mock_GetTokenInformation(const char *file, const int line, const char *func, HANDLE token_handle,
                              TOKEN_INFORMATION_CLASS token_information_class, LPVOID token_information,
                              DWORD token_information_length, PDWORD return_length)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetTokenInformation(file, line, func, token_handle, token_information_class,
                                                      token_information, token_information_length, return_length);
    }
    else
    {
        mock_ret = delegate_real_GetTokenInformation(file, line, func, token_handle, token_information_class,
                                                     token_information, token_information_length, return_length);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetTokenInformation 0x%p, %d", (void *)token_handle, (int)token_information_class);
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
