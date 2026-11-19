#ifdef _WIN32

    #include <mock_stdio.h>
    #include <test_com.h>

using namespace testing;

int delegate_real__fileno(const char *file, const int line, const char *func, FILE *stream)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _fileno(stream);
}

int mock__fileno(const char *file, const int line, const char *func, FILE *stream)
{
    int mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->_fileno(file, line, func, stream);
    }
    else
    {
        mock_ret = delegate_real__fileno(file, line, func, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _fileno 0x%p", (void *)stream);
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
