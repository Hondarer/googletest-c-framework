#ifdef _WIN32

    #include <test_com.h>
    #include <mock_stdio.h>

using namespace testing;

int delegate_fake__fseeki64(const char *file, const int line, const char *func, FILE *stream, __int64 offset,
                            int whence)
{
    (void)file;
    (void)line;
    (void)func;
    (void)stream;
    (void)offset;
    (void)whence;

    return 0;
}

int delegate_real__fseeki64(const char *file, const int line, const char *func, FILE *stream, __int64 offset,
                            int whence)
{
    (void)file;
    (void)line;
    (void)func;

    return _fseeki64(stream, offset, whence);
}

int mock__fseeki64(const char *file, const int line, const char *func, FILE *stream, __int64 offset, int whence)
{
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->_fseeki64(file, line, func, stream, offset, whence);
    }
    else
    {
        mock_ret = delegate_real__fseeki64(file, line, func, stream, offset, whence);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _fseeki64 0x%p, %lld, %d", (void *)stream, (long long)offset, whence);
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

#endif /* _WIN32 */
