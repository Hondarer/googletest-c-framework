#include <test_com.h>
#include <mock_zlib.h>

using namespace testing;

int delegate_real_inflateInit2_(const char *file, const int line, const char *func, z_streamp strm, int windowBits,
                                const char *version, int stream_size)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return inflateInit2_(strm, windowBits, version, stream_size);
}

int mock_inflateInit2_(const char *file, const int line, const char *func, z_streamp strm, int windowBits,
                       const char *version, int stream_size)
{
    int mock_ret;

    if (_mock_zlib != nullptr)
    {
        mock_ret = _mock_zlib->inflateInit2_(file, line, func, strm, windowBits, version, stream_size);
    }
    else
    {
        mock_ret = delegate_real_inflateInit2_(file, line, func, strm, windowBits, version, stream_size);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > inflateInit2_");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
