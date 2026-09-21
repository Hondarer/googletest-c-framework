#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_GetUserPreferredUILanguages(const char *file, const int line, const char *func, DWORD flags,
                                               PULONG language_count, PZZWSTR languages, PULONG language_size)
{
    (void)file;
    (void)line;
    (void)func;

    return GetUserPreferredUILanguages(flags, language_count, languages, language_size);
}

BOOL mock_GetUserPreferredUILanguages(const char *file, const int line, const char *func, DWORD flags,
                                      PULONG language_count, PZZWSTR languages, PULONG language_size)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->GetUserPreferredUILanguages(file, line, func, flags, language_count, languages,
                                                              language_size);
    }
    else
    {
        mock_ret = delegate_real_GetUserPreferredUILanguages(file, line, func, flags, language_count, languages,
                                                             language_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetUserPreferredUILanguages 0x%08lx, 0x%p", (unsigned long)flags, (void *)languages);
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
