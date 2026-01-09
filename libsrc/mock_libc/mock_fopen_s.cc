#include <test_com.h>
#include <mock_stdio.h>

#include <string.h>
#include <errno.h>

using namespace testing;

errno_t delegate_fake_fopen_s(const char *file, const int line, const char *func, FILE **pFile, const char *filename, const char *modes)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;
    (void)filename;
    (void)modes;

    if (pFile == NULL)
    {
        return EINVAL;
    }

    FILE *fp = (FILE *)malloc(sizeof(FILE));
    *pFile = fp;

    return 0;
}

errno_t delegate_real_fopen_s(const char *file, const int line, const char *func, FILE **pFile, const char *filename, const char *modes)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (pFile == NULL)
    {
        return EINVAL;
    }

#ifndef _WIN32
    // Linux (fopen_s is not available, use fopen instead)
    FILE *fp = fopen(filename, modes);
    if (fp == NULL)
    {
        *pFile = NULL;
        return errno;
    }
    *pFile = fp;
    return 0;
#else
    // Windows
    return fopen_s(pFile, filename, modes);
#endif
}

errno_t mock_fopen_s(const char *file, const int line, const char *func, FILE **pFile, const char *filename, const char *modes)
{
    errno_t err;

    if (_mock_stdio != nullptr)
    {
        err = _mock_stdio->fopen_s(file, line, func, pFile, filename, modes);
    }
    else
    {
        err = delegate_real_fopen_s(file, line, func, pFile, filename, modes);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fopen_s %s, %c", filename, *modes);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (err != 0 || *pFile == NULL)
            {
                printf(" from %s:%d -> error %d\n", file, line, err);
            }
            else
            {
                printf(" from %s:%d -> 0x%p\n", file, line, (void *)*pFile);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return err;
}
