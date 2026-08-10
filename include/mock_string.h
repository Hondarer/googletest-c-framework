#ifndef _MOCK_STRING_H
#define _MOCK_STRING_H

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern void *mock_memset(const char *, const int, const char *, void *, int, size_t);
#ifndef _WIN32
    /* strdup / strerror_r は POSIX のみ。Windows は _strdup / strerror_s を使うためモック対象外 */
    extern char *mock_strdup(const char *, const int, const char *, const char *);
    extern int mock_strerror_r(const char *, const int, const char *, int, char *, size_t);
#endif // _WIN32

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STRING_H

    #define memset(s, c, n) mock_memset(__FILE__, __LINE__, __func__, s, c, n)
    #ifndef _WIN32
        #define strdup(s)                 mock_strdup(__FILE__, __LINE__, __func__, s)
        #define strerror_r(errnum, b, sz) mock_strerror_r(__FILE__, __LINE__, __func__, errnum, b, sz)
    #endif // _WIN32

#else // _IN_OVERRIDE_HEADER_STRING_H

    #ifndef _WIN32
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
    #endif // _WIN32
    #include <gmock/gmock.h>
    #ifndef _WIN32
        #pragma GCC diagnostic pop
    #endif // _WIN32

extern void *delegate_real_memset(const char *, const int, const char *, void *, int, size_t);
    #ifndef _WIN32
extern char *delegate_real_strdup(const char *, const int, const char *, const char *);
extern int delegate_real_strerror_r(const char *, const int, const char *, int, char *, size_t);
    #endif // _WIN32

class Mock_string
{
  public:
    MOCK_METHOD(void *, memset, (const char *, const int, const char *, void *, int, size_t));
    #ifndef _WIN32
    MOCK_METHOD(char *, strdup, (const char *, const int, const char *, const char *));
    MOCK_METHOD(int, strerror_r, (const char *, const int, const char *, int, char *, size_t));
    #endif // _WIN32

    Mock_string();
    ~Mock_string();
};

extern Mock_string *_mock_string;

#endif // _IN_OVERRIDE_HEADER_STRING_H

#endif // _MOCK_STRING_H
