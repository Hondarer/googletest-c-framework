#ifdef _WIN32

#include <test_com.h>
#include <mock_windows.h>

#include <string.h>

using namespace testing;

void delegate_fake_GetSystemTimeAsFileTime(const char *file, const int line, const char *func, LPFILETIME file_time)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (file_time != NULL)
    {
        memset(file_time, 0, sizeof(*file_time));
    }
}

void delegate_real_GetSystemTimeAsFileTime(const char *file, const int line, const char *func, LPFILETIME file_time)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    GetSystemTimeAsFileTime(file_time);
}

void mock_GetSystemTimeAsFileTime(const char *file, const int line, const char *func, LPFILETIME file_time)
{
    if (_mock_windows != nullptr)
    {
        _mock_windows->GetSystemTimeAsFileTime(file, line, func, file_time);
    }
    else
    {
        delegate_real_GetSystemTimeAsFileTime(file, line, func, file_time);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > GetSystemTimeAsFileTime 0x%p", (void *)file_time);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (file_time == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> %lu, %lu\n",
                       file, line,
                       (unsigned long)file_time->dwHighDateTime,
                       (unsigned long)file_time->dwLowDateTime);
            }
        }
        else
        {
            printf("\n");
        }
    }
}

#endif // _WIN32
