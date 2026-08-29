#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_gethostname(const char *file, const int line, const char *func, char *name, size_t len)
{
    (void)file;
    (void)line;
    (void)func;
    return gethostname(name, len);
}

int mock_gethostname(const char *file, const int line, const char *func, char *name, size_t len)
{
    int mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->gethostname(file, line, func, name, len);
    }
    else
    {
        mock_ret = delegate_real_gethostname(file, line, func, name, len);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > gethostname 0x%p, %zu", (void *)name, len);
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
