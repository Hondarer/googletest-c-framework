#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

int delegate_real_GetUserDefaultLocaleName(const char *file, const int line, const char *func, LPWSTR locale_name,
                                           int locale_name_count)
{
    (void)file;
    (void)line;
    (void)func;

    return GetUserDefaultLocaleName(locale_name, locale_name_count);
}

int mock_GetUserDefaultLocaleName(const char *file, const int line, const char *func, LPWSTR locale_name,
                                  int locale_name_count)
{
    int mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetUserDefaultLocaleName(file, line, func, locale_name, locale_name_count);
    }
    else
    {
        mock_ret = delegate_real_GetUserDefaultLocaleName(file, line, func, locale_name, locale_name_count);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetUserDefaultLocaleName 0x%p, %d", (void *)locale_name, locale_name_count);
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
