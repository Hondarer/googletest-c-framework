#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

UINT delegate_real_GetConsoleCP(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return GetConsoleCP();
}

UINT mock_GetConsoleCP(const char *file, const int line, const char *func)
{
    UINT mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetConsoleCP(file, line, func);
    }
    else
    {
        mock_ret = delegate_real_GetConsoleCP(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetConsoleCP");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %u\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
