#ifdef _WIN32

    #include <mock_stdio.h>
    #include <test_com.h>

using namespace testing;

int delegate_real_setvbuf(const char *file, const int line, const char *func, FILE *stream, char *buffer, int mode,
                          size_t size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return setvbuf(stream, buffer, mode, size);
}

int mock_setvbuf(const char *file, const int line, const char *func, FILE *stream, char *buffer, int mode, size_t size)
{
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->setvbuf(file, line, func, stream, buffer, mode, size);
    }
    else
    {
        mock_ret = delegate_real_setvbuf(file, line, func, stream, buffer, mode, size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > setvbuf 0x%p, 0x%p, %d, %zu", (void *)stream, (void *)buffer, mode, size);
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
