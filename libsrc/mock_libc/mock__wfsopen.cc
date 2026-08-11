#ifdef _WIN32

    #include <test_com.h>
    #include <mock_stdio.h>

using namespace testing;

FILE *delegate_fake__wfsopen(const char *file, const int line, const char *func, const wchar_t *filename,
                             const wchar_t *modes, int shflag)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)filename;
    (void)modes;
    (void)shflag;

    return (FILE *)malloc(sizeof(FILE));
}

FILE *delegate_real__wfsopen(const char *file, const int line, const char *func, const wchar_t *filename,
                             const wchar_t *modes, int shflag)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return _wfsopen(filename, modes, shflag);
}

FILE *mock__wfsopen(const char *file, const int line, const char *func, const wchar_t *filename, const wchar_t *modes,
                    int shflag)
{
    FILE *fp;

    if (_mock_stdio != nullptr)
    {
        fp = _mock_stdio->_wfsopen(file, line, func, filename, modes, shflag);
    }
    else
    {
        fp = delegate_real__wfsopen(file, line, func, filename, modes, shflag);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _wfsopen %ls, %ls, %d", filename, modes, shflag);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (fp == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> 0x%p\n", file, line, (void *)fp);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return fp;
}

#endif // _WIN32
