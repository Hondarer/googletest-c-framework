#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_InitializeProcThreadAttributeList(const char *file, const int line, const char *func,
                                                     LPPROC_THREAD_ATTRIBUTE_LIST attribute_list, DWORD attribute_count,
                                                     DWORD flags, PSIZE_T size)
{
    (void)file;
    (void)line;
    (void)func;

    return InitializeProcThreadAttributeList(attribute_list, attribute_count, flags, size);
}

BOOL mock_InitializeProcThreadAttributeList(const char *file, const int line, const char *func,
                                            LPPROC_THREAD_ATTRIBUTE_LIST attribute_list, DWORD attribute_count,
                                            DWORD flags, PSIZE_T size)
{
    BOOL mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->InitializeProcThreadAttributeList(file, line, func, attribute_list, attribute_count, flags,
                                                               size);
    }
    else
    {
        mock_ret = delegate_real_InitializeProcThreadAttributeList(file, line, func, attribute_list, attribute_count, flags,
                                                              size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > InitializeProcThreadAttributeList 0x%p, %lu, %lu, 0x%p", (void *)attribute_list, attribute_count,
               flags, (void *)size);
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
