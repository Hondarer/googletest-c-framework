#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetComputerNameExW(const char *file, const int line, const char *func,
                                      COMPUTER_NAME_FORMAT name_type, LPWSTR buffer, LPDWORD size)
{
    (void)file;
    (void)line;
    (void)func;

    return GetComputerNameExW(name_type, buffer, size);
}

BOOL mock_GetComputerNameExW(const char *file, const int line, const char *func, COMPUTER_NAME_FORMAT name_type,
                             LPWSTR buffer, LPDWORD size)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetComputerNameExW(file, line, func, name_type, buffer, size);
    }
    else
    {
        mock_ret = delegate_real_GetComputerNameExW(file, line, func, name_type, buffer, size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetComputerNameExW %d, 0x%p", (int)name_type, (void *)buffer);
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
