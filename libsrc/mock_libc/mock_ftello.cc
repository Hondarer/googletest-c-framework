#ifndef _WIN32

    #include <test_com.h>
    #include <mock_stdio.h>

using namespace testing;

off_t delegate_fake_ftello(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;
    (void)stream;

    return 0;
}

off_t delegate_real_ftello(const char *file, const int line, const char *func, FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;

    return ftello(stream);
}

off_t mock_ftello(const char *file, const int line, const char *func, FILE *stream)
{
    off_t rtc;

    if (_mock_stdio != nullptr)
    {
        rtc = _mock_stdio->ftello(file, line, func, stream);
    }
    else
    {
        rtc = delegate_real_ftello(file, line, func, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ftello 0x%p", (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lld\n", file, line, (long long)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}

#endif /* !_WIN32 */
