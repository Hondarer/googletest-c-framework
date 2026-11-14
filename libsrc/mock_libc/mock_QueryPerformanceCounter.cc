#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_fake_QueryPerformanceCounter(const char *file, const int line, const char *func,
                                            LARGE_INTEGER *counter)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (counter == nullptr)
    {
        return FALSE;
    }

    counter->QuadPart = 0;
    return TRUE;
}

BOOL delegate_real_QueryPerformanceCounter(const char *file, const int line, const char *func,
                                            LARGE_INTEGER *counter)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return QueryPerformanceCounter(counter);
}

BOOL mock_QueryPerformanceCounter(const char *file, const int line, const char *func, LARGE_INTEGER *counter)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->QueryPerformanceCounter(file, line, func, counter);
    }
    else
    {
        mock_ret = delegate_real_QueryPerformanceCounter(file, line, func, counter);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > QueryPerformanceCounter 0x%p", (void *)counter);
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
