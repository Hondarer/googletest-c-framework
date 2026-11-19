#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_SetConsoleCP(const char *file, const int line, const char *func, UINT code_page_id)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return SetConsoleCP(code_page_id);
}

BOOL mock_SetConsoleCP(const char *file, const int line, const char *func, UINT code_page_id)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->SetConsoleCP(file, line, func, code_page_id);
    }
    else
    {
        mock_ret = delegate_real_SetConsoleCP(file, line, func, code_page_id);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > SetConsoleCP %u", code_page_id);
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
