#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

BOOL delegate_real_UpdateProcThreadAttribute(const char *file, const int line, const char *func,
                                             LPPROC_THREAD_ATTRIBUTE_LIST attribute_list, DWORD flags,
                                             DWORD_PTR attribute, PVOID value, SIZE_T size, PVOID previous,
                                             PSIZE_T return_size)
{
    (void)file;
    (void)line;
    (void)func;

    return UpdateProcThreadAttribute(attribute_list, flags, attribute, value, size, previous, return_size);
}

BOOL mock_UpdateProcThreadAttribute(const char *file, const int line, const char *func,
                                    LPPROC_THREAD_ATTRIBUTE_LIST attribute_list, DWORD flags, DWORD_PTR attribute,
                                    PVOID value, SIZE_T size, PVOID previous, PSIZE_T return_size)
{
    BOOL rtc;

    if (_mock_windows != nullptr)
    {
        rtc = _mock_windows->UpdateProcThreadAttribute(file, line, func, attribute_list, flags, attribute, value, size,
                                                       previous, return_size);
    }
    else
    {
        rtc = delegate_real_UpdateProcThreadAttribute(file, line, func, attribute_list, flags, attribute, value, size,
                                                      previous, return_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > UpdateProcThreadAttribute 0x%p, %lu, %llu, 0x%p, %llu", (void *)attribute_list, flags,
               (unsigned long long)attribute, value, (unsigned long long)size);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
