#ifndef _WIN32

    #include <test_com.h>
    #include <mock_stdio.h>

using namespace testing;

int delegate_fake_fseeko(const char *file, const int line, const char *func, FILE *stream, off_t offset, int whence)
{
    (void)file;
    (void)line;
    (void)func;
    (void)stream;
    (void)offset;
    (void)whence;

    return 0;
}

int delegate_real_fseeko(const char *file, const int line, const char *func, FILE *stream, off_t offset, int whence)
{
    (void)file;
    (void)line;
    (void)func;

    return fseeko(stream, offset, whence);
}

int mock_fseeko(const char *file, const int line, const char *func, FILE *stream, off_t offset, int whence)
{
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->fseeko(file, line, func, stream, offset, whence);
    }
    else
    {
        mock_ret = delegate_real_fseeko(file, line, func, stream, offset, whence);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fseeko 0x%p, %lld, %d", (void *)stream, (long long)offset, whence);
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

#endif /* !_WIN32 */
