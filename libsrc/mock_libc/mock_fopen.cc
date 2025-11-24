#include <test_com.h>
#include <mock_stdio.h>

#include <string.h>

using namespace testing;

FILE *delegate_fake_fopen(const char *file, const int line, const char *func, const char *filename, const char *modes)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)filename;
    (void)modes;

    FILE *fp = (FILE *)malloc(sizeof(FILE));

    return fp;
}

FILE *delegate_real_fopen(const char *file, const int line, const char *func, const char *filename, const char *modes)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

#ifndef _WIN32
    // Linux
    return fopen(filename, modes);
#else
    // Windows
    FILE *fp = NULL;
    errno_t err = fopen_s(&fp, filename, modes);
    if (err != 0)
    {
        return NULL;
    }
    return fp;
#endif
}

FILE *mock_fopen(const char *file, const int line, const char *func, const char *filename, const char *modes)
{
    FILE *fp;

    if (_mock_stdio != nullptr)
    {
        fp = _mock_stdio->fopen(file, line, func, filename, modes);
    }
    else
    {
        fp = delegate_real_fopen(file, line, func, filename, modes);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fopen %s, %c", filename, *modes);
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
