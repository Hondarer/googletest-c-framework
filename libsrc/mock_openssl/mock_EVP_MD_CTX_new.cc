#include <test_com.h>
#include <mock_openssl.h>

using namespace testing;

EVP_MD_CTX *delegate_real_EVP_MD_CTX_new(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return EVP_MD_CTX_new();
}

EVP_MD_CTX *mock_EVP_MD_CTX_new(const char *file, const int line, const char *func)
{
    EVP_MD_CTX *result;

    if (_mock_openssl != nullptr)
    {
        result = _mock_openssl->EVP_MD_CTX_new(file, line, func);
    }
    else
    {
        result = delegate_real_EVP_MD_CTX_new(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_MD_CTX_new");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}
