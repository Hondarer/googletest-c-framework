#ifndef _MOCK_ZLIB_H
#define _MOCK_ZLIB_H

/* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
#ifndef _IN_OVERRIDE_HEADER_ZLIB_H
    #include <zlib.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    extern int mock_deflateInit2_(const char *, const int, const char *, z_streamp, int, int, int, int, int,
                                  const char *, int);
    extern int mock_inflateInit2_(const char *, const int, const char *, z_streamp, int, const char *, int);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_ZLIB_H

    /* zlib の deflateInit2 / inflateInit2 はマクロで deflateInit2_ / inflateInit2_ へ展開される。
     * 差し替えは実体側の名前で行う。
     * see: https://zlib.net/manual.html */
    #define deflateInit2_(strm, level, method, windowBits, memLevel, strategy, version, stream_size)         \
        mock_deflateInit2_(__FILE__, __LINE__, __func__, strm, level, method, windowBits, memLevel, strategy, \
                           version, stream_size)
    #define inflateInit2_(strm, windowBits, version, stream_size) \
        mock_inflateInit2_(__FILE__, __LINE__, __func__, strm, windowBits, version, stream_size)

#else // _IN_OVERRIDE_HEADER_ZLIB_H

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpadded"
    #include <gmock/gmock.h>
    #pragma GCC diagnostic pop

extern int delegate_real_deflateInit2_(const char *, const int, const char *, z_streamp, int, int, int, int, int,
                                       const char *, int);
extern int delegate_real_inflateInit2_(const char *, const int, const char *, z_streamp, int, const char *, int);

class Mock_zlib
{
  public:
    MOCK_METHOD(int, deflateInit2_,
                (const char *, const int, const char *, z_streamp, int, int, int, int, int, const char *, int));
    MOCK_METHOD(int, inflateInit2_, (const char *, const int, const char *, z_streamp, int, const char *, int));

    Mock_zlib();
    ~Mock_zlib();
};

extern Mock_zlib *_mock_zlib;

#endif // _IN_OVERRIDE_HEADER_ZLIB_H

#endif // _MOCK_ZLIB_H
