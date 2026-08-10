#include <test_com.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_RAND_bytes(const char *file, const int line, const char *func, unsigned char *buf, int num)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return RAND_bytes(buf, num);
}

int mock_RAND_bytes(const char *file, const int line, const char *func, unsigned char *buf, int num)
{
    int result;

    if (_mock_openssl != nullptr)
    {
        result = _mock_openssl->RAND_bytes(file, line, func, buf, num);
    }
    else
    {
        result = delegate_real_RAND_bytes(file, line, func, buf, num);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > RAND_bytes");
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
