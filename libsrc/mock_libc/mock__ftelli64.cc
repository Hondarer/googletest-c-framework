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
    __int64 rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->_ftelli64(file, line, func, stream);
    }
    else
    {
        rtc = delegate_real__ftelli64(file, line, func, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _ftelli64 0x%p", (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lld\n", file, line, (long long)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif /* _WIN32 */
