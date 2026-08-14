#ifdef _WIN32

    #include <test_com.h>
    #include <mock_stdio.h>

using namespace testing;

__int64 delegate_fake__ftelli64(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;
    (void)stream;

    return 0;
}

__int64 delegate_real__ftelli64(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;

    return _ftelli64(stream);
}

__int64 mock__ftelli64(const char *file, const int line, const char *func, FILE *stream)
{
    __int64 mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->_ftelli64(file, line, func, stream);
    }
    else
    {
        mock_ret = delegate_real__ftelli64(file, line, func, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _ftelli64 0x%p", (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lld\n", file, line, (long long)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif /* _WIN32 */
