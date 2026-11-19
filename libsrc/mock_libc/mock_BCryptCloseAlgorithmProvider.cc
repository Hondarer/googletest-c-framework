#ifdef _WIN32

    #include <mock_bcrypt.h>
    #include <test_com.h>

    #pragma comment(lib, "bcrypt.lib")

using namespace testing;

NTSTATUS delegate_real_BCryptCloseAlgorithmProvider(const char *file, const int line, const char *func,
                                                    BCRYPT_ALG_HANDLE algorithm, ULONG flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return BCryptCloseAlgorithmProvider(algorithm, flags);
}

NTSTATUS mock_BCryptCloseAlgorithmProvider(const char *file, const int line, const char *func,
                                           BCRYPT_ALG_HANDLE algorithm, ULONG flags)
{
    NTSTATUS mock_ret;

    if (_mock_bcrypt != nullptr)
    {
        mock_ret = _mock_bcrypt->BCryptCloseAlgorithmProvider(file, line, func, algorithm, flags);
    }
    else
    {
        mock_ret = delegate_real_BCryptCloseAlgorithmProvider(file, line, func, algorithm, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > BCryptCloseAlgorithmProvider 0x%p, 0x%lx", (void *)algorithm, flags);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%08lx\n", file, line, (unsigned long)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
