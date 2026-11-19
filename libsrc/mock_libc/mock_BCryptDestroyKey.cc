#ifdef _WIN32

    #include <mock_bcrypt.h>
    #include <test_com.h>

    #pragma comment(lib, "bcrypt.lib")

using namespace testing;

NTSTATUS delegate_real_BCryptDestroyKey(const char *file, const int line, const char *func, BCRYPT_KEY_HANDLE key)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return BCryptDestroyKey(key);
}

NTSTATUS mock_BCryptDestroyKey(const char *file, const int line, const char *func, BCRYPT_KEY_HANDLE key)
{
    NTSTATUS mock_ret;

    if (_mock_bcrypt != nullptr)
    {
        mock_ret = _mock_bcrypt->BCryptDestroyKey(file, line, func, key);
    }
    else
    {
        mock_ret = delegate_real_BCryptDestroyKey(file, line, func, key);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > BCryptDestroyKey 0x%p", (void *)key);
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
