#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_mkstemp(const char *file, const int line, const char *func, char *tmpl)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return mkstemp(tmpl);
}

int mock_mkstemp(const char *file, const int line, const char *func, char *tmpl)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->mkstemp(file, line, func, tmpl);
    }
    else
    {
        mock_ret = delegate_real_mkstemp(file, line, func, tmpl);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > mkstemp %s", tmpl);
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
