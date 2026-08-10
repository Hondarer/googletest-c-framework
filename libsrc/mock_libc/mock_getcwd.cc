#ifndef _WIN32

    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

char *delegate_real_getcwd(const char *file, const int line, const char *func, char *buf, size_t size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getcwd(buf, size);
}

char *mock_getcwd(const char *file, const int line, const char *func, char *buf, size_t size)
{
    char *result;

    if (_mock_unistd != nullptr)
    {
        result = _mock_unistd->getcwd(file, line, func, buf, size);
    }
    else
    {
        result = delegate_real_getcwd(file, line, func, buf, size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > getcwd");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
