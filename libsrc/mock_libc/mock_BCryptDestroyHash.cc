#ifdef _WIN32

    #include <mock_bcrypt.h>
    #include <test_com.h>

    #pragma comment(lib, "bcrypt.lib")

using namespace testing;

NTSTATUS delegate_real_BCryptDestroyHash(const char *file, const int line, const char *func, BCRYPT_HASH_HANDLE hash)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return BCryptDestroyHash(hash);
}

NTSTATUS mock_BCryptDestroyHash(const char *file, const int line, const char *func, BCRYPT_HASH_HANDLE hash)
{
    NTSTATUS mock_ret;

    if (_mock_bcrypt != nullptr)
    {
        mock_ret = _mock_bcrypt->BCryptDestroyHash(file, line, func, hash);
    }
    else
    {
        mock_ret = delegate_real_BCryptDestroyHash(file, line, func, hash);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > BCryptDestroyHash 0x%p", (void *)hash);
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
