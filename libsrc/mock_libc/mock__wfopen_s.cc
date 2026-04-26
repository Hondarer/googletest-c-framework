#ifdef _WIN32

#include <test_com.h>
#include <mock_stdio.h>

#include <errno.h>

using namespace testing;

errno_t delegate_fake__wfopen_s(const char *file, const int line, const char *func,
                                 FILE **pFile, const wchar_t *filename, const wchar_t *modes)
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

    *pFile = (FILE *)malloc(sizeof(FILE));
    return 0;
}

errno_t delegate_real__wfopen_s(const char *file, const int line, const char *func,
                                 FILE **pFile, const wchar_t *filename, const wchar_t *modes)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (pFile == NULL)
    {
        return EINVAL;
    }

    return _wfopen_s(pFile, filename, modes);
}

errno_t mock__wfopen_s(const char *file, const int line, const char *func,
                        FILE **pFile, const wchar_t *filename, const wchar_t *modes)
{
    errno_t rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->_wfopen_s(file, line, func, pFile, filename, modes);
    }
    else
    {
        rtc = delegate_real__wfopen_s(file, line, func, pFile, filename, modes);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > _wfopen_s %ls, %ls", filename, modes);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif // _WIN32
