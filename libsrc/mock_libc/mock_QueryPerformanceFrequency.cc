#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_fake_QueryPerformanceFrequency(const char *file, const int line, const char *func,
                                            LARGE_INTEGER *frequency)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (frequency == nullptr)
    {
        return FALSE;
    }

    frequency->QuadPart = 1000000000LL;
    return TRUE;
}

BOOL delegate_real_QueryPerformanceFrequency(const char *file, const int line, const char *func,
                                            LARGE_INTEGER *frequency)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return QueryPerformanceFrequency(frequency);
}

BOOL mock_QueryPerformanceFrequency(const char *file, const int line, const char *func, LARGE_INTEGER *frequency)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->QueryPerformanceFrequency(file, line, func, frequency);
    }
    else
    {
        mock_ret = delegate_real_QueryPerformanceFrequency(file, line, func, frequency);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > QueryPerformanceFrequency 0x%p", (void *)frequency);
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
