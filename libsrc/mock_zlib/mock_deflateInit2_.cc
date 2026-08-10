#include <test_com.h>
#include <mock_zlib.h>

using namespace testing;

int delegate_real_deflateInit2_(const char *file, const int line, const char *func, z_streamp strm, int level,
                                int method, int windowBits, int memLevel, int strategy, const char *version,
                                int stream_size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return deflateInit2_(strm, level, method, windowBits, memLevel, strategy, version, stream_size);
}

int mock_deflateInit2_(const char *file, const int line, const char *func, z_streamp strm, int level, int method,
                       int windowBits, int memLevel, int strategy, const char *version, int stream_size)
{
    int result;

    if (_mock_zlib != nullptr)
    {
        result = _mock_zlib->deflateInit2_(file, line, func, strm, level, method, windowBits, memLevel, strategy,
                                           version, stream_size);
    }
    else
    {
        result = delegate_real_deflateInit2_(file, line, func, strm, level, method, windowBits, memLevel, strategy,
                                             version, stream_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > deflateInit2_");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}
