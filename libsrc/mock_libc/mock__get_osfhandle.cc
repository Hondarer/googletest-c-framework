#ifdef _WIN32

    #include <mock_unistd.h>
    #include <test_com.h>

using namespace testing;

intptr_t delegate_real__get_osfhandle(const char *file, const int line, const char *func, int fd)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _get_osfhandle(fd);
}

intptr_t mock__get_osfhandle(const char *file, const int line, const char *func, int fd)
{
    intptr_t mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->_get_osfhandle(file, line, func, fd);
    }
    else
    {
        mock_ret = delegate_real__get_osfhandle(file, line, func, fd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _get_osfhandle %d", fd);
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
