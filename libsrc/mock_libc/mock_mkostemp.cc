#ifndef _WIN32

    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif

    #include <stdlib.h>
    #include <test_com.h>
    #include <mock_unistd.h>

using namespace testing;

int delegate_real_mkostemp(const char *file, const int line, const char *func, char *tmpl, int flags)
{
    (void)file;
    (void)line;
    (void)func;

    return mkostemp(tmpl, flags);
}

int mock_mkostemp(const char *file, const int line, const char *func, char *tmpl, int flags)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->mkostemp(file, line, func, tmpl, flags);
    }
    else
    {
        mock_ret = delegate_real_mkostemp(file, line, func, tmpl, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > mkostemp %s, %d", tmpl, flags);
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
