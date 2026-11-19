#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

void delegate_real_OutputDebugStringW(const char *file, const int line, const char *func, LPCWSTR output_string)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    OutputDebugStringW(output_string);
}

void mock_OutputDebugStringW(const char *file, const int line, const char *func, LPCWSTR output_string)
{
    if (_mock_windows != nullptr)
    {
        _mock_windows->OutputDebugStringW(file, line, func, output_string);
    }
    else
    {
        delegate_real_OutputDebugStringW(file, line, func, output_string);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > OutputDebugStringW 0x%p", (const void *)output_string);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}

#endif // _WIN32
