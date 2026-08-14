#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

void delegate_real_DeleteProcThreadAttributeList(const char *file, const int line, const char *func,
                                                 LPPROC_THREAD_ATTRIBUTE_LIST attribute_list)
{
    (void)file;
    (void)line;
    (void)func;

    DeleteProcThreadAttributeList(attribute_list);
}

void mock_DeleteProcThreadAttributeList(const char *file, const int line, const char *func,
                                        LPPROC_THREAD_ATTRIBUTE_LIST attribute_list)
{
    if (_mock_windows != nullptr)
    {
        _mock_windows->DeleteProcThreadAttributeList(file, line, func, attribute_list);
    }
    else
    {
        delegate_real_DeleteProcThreadAttributeList(file, line, func, attribute_list);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > DeleteProcThreadAttributeList 0x%p", (void *)attribute_list);
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
