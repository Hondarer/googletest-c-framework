#ifdef _WIN32

    #include <mock_unistd.h>
    #include <test_com.h>

using namespace testing;

int delegate_real__open_osfhandle(const char *file, const int line, const char *func, intptr_t os_handle, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _open_osfhandle(os_handle, flags);
}

int mock__open_osfhandle(const char *file, const int line, const char *func, intptr_t os_handle, int flags)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->_open_osfhandle(file, line, func, os_handle, flags);
    }
    else
    {
        mock_ret = delegate_real__open_osfhandle(file, line, func, os_handle, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _open_osfhandle 0x%p, 0x%x", (void *)os_handle, flags);
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
