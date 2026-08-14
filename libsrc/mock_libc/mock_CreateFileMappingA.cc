#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

HANDLE delegate_real_CreateFileMappingA(const char *file, const int line, const char *func, HANDLE mapping_file,
                                        LPSECURITY_ATTRIBUTES attributes, DWORD protect, DWORD size_high,
                                        DWORD size_low, LPCSTR name)
{
    (void)file;
    (void)line;
    (void)func;

    return CreateFileMappingA(mapping_file, attributes, protect, size_high, size_low, name);
}

HANDLE mock_CreateFileMappingA(const char *file, const int line, const char *func, HANDLE mapping_file,
                               LPSECURITY_ATTRIBUTES attributes, DWORD protect, DWORD size_high, DWORD size_low,
                               LPCSTR name)
{
    HANDLE mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->CreateFileMappingA(file, line, func, mapping_file, attributes, protect, size_high,
                                                size_low, name);
    }
    else
    {
        mock_ret = delegate_real_CreateFileMappingA(file, line, func, mapping_file, attributes, protect, size_high, size_low,
                                               name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > CreateFileMappingA 0x%p, 0x%p, %lu, %lu, %lu, %s", (void *)mapping_file, (void *)attributes,
               protect, size_high, size_low, (name != NULL) ? name : "(null)");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
