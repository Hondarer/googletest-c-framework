#ifdef _WIN32

    #ifndef _MOCK_DIRECT_H
        #define _MOCK_DIRECT_H

        #include <wchar.h>

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern int mock_wmkdir(const char *, const int, const char *, const wchar_t *);
    extern int mock_wrmdir(const char *, const int, const char *, const wchar_t *);

        #ifdef __cplusplus
}
        #endif

        #ifdef _IN_OVERRIDE_HEADER_DIRECT_H
            #define _wmkdir(path) mock_wmkdir(__FILE__, __LINE__, __func__, path)
            #define _wrmdir(path) mock_wrmdir(__FILE__, __LINE__, __func__, path)
        #else // _IN_OVERRIDE_HEADER_DIRECT_H
            #include <gmock/gmock.h>

extern int delegate_real_wmkdir(const char *, const int, const char *, const wchar_t *);
extern int delegate_real_wrmdir(const char *, const int, const char *, const wchar_t *);

class Mock_direct
{
  public:
    MOCK_METHOD(int, wmkdir, (const char *, const int, const char *, const wchar_t *));
    MOCK_METHOD(int, wrmdir, (const char *, const int, const char *, const wchar_t *));

    Mock_direct();
    virtual ~Mock_direct();
};

extern Mock_direct *_mock_direct;
        #endif // _IN_OVERRIDE_HEADER_DIRECT_H

    #endif // _MOCK_DIRECT_H

#endif // _WIN32
