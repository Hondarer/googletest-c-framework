#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

int delegate_real_CompareStringOrdinal(const char *file, const int line, const char *func, LPCWCH string1, int count1,
                                       LPCWCH string2, int count2, BOOL ignore_case)
{
    (void)file;
    (void)line;
    (void)func;

    return CompareStringOrdinal(string1, count1, string2, count2, ignore_case);
}

int mock_CompareStringOrdinal(const char *file, const int line, const char *func, LPCWCH string1, int count1,
                              LPCWCH string2, int count2, BOOL ignore_case)
{
    int mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->CompareStringOrdinal(file, line, func, string1, count1, string2, count2, ignore_case);
    }
    else
    {
        mock_ret = delegate_real_CompareStringOrdinal(file, line, func, string1, count1, string2, count2, ignore_case);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CompareStringOrdinal 0x%p, %d, 0x%p, %d, %d", (const void *)string1, count1,
               (const void *)string2, count2, ignore_case);
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
