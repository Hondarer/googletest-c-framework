#ifndef _MOCK_STRING_H
#define _MOCK_STRING_H

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern void *mock_memcpy(const char *, const int, const char *, void *, const void *, size_t);
    extern void *mock_memmove(const char *, const int, const char *, void *, const void *, size_t);
    extern void *mock_memset(const char *, const int, const char *, void *, int, size_t);
    extern char *mock_strchr(const char *, const int, const char *, const char *, int);
    extern int mock_strcmp(const char *, const int, const char *, const char *, const char *);
    extern size_t mock_strlen(const char *, const int, const char *, const char *);
    extern int mock_strncmp(const char *, const int, const char *, const char *, const char *, size_t);
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
    /* C++ では STL のインライン関数 (std::char_traits など) が :: 修飾でこれらを呼ぶため置換しない。
     * 置換すると STL 内部の呼び出しが mock を経由し、呼び出し回数の検証を乱し、mock_libc のリンクも要求する。
     * see: https://github.com/microsoft/STL/tree/main/stl/inc */
    #ifndef __cplusplus
        #define memcpy(d, s, n)    mock_memcpy(__FILE__, __LINE__, __func__, d, s, n)
        #define memmove(d, s, n)   mock_memmove(__FILE__, __LINE__, __func__, d, s, n)
        #define strchr(s, c)       mock_strchr(__FILE__, __LINE__, __func__, s, c)
        #define strcmp(s1, s2)     mock_strcmp(__FILE__, __LINE__, __func__, s1, s2)
        #define strlen(s)          mock_strlen(__FILE__, __LINE__, __func__, s)
        #define strncmp(s1, s2, n) mock_strncmp(__FILE__, __LINE__, __func__, s1, s2, n)
    #endif // __cplusplus
    #ifndef _WIN32
        #define strdup(s)                 mock_strdup(__FILE__, __LINE__, __func__, s)
        #define strerror_r(errnum, b, sz) mock_strerror_r(__FILE__, __LINE__, __func__, errnum, b, sz)
    #endif // _WIN32

#else // _IN_OVERRIDE_HEADER_STRING_H

    #include <gmock/gmock.h>

extern void *delegate_real_memcpy(const char *, const int, const char *, void *, const void *, size_t);
extern void *delegate_real_memmove(const char *, const int, const char *, void *, const void *, size_t);
extern void *delegate_real_memset(const char *, const int, const char *, void *, int, size_t);
extern char *delegate_real_strchr(const char *, const int, const char *, const char *, int);
extern int delegate_real_strcmp(const char *, const int, const char *, const char *, const char *);
extern size_t delegate_real_strlen(const char *, const int, const char *, const char *);
extern int delegate_real_strncmp(const char *, const int, const char *, const char *, const char *, size_t);
    #ifndef _WIN32
extern char *delegate_real_strdup(const char *, const int, const char *, const char *);
extern int delegate_real_strerror_r(const char *, const int, const char *, int, char *, size_t);
    #endif // _WIN32

class Mock_string
{
  public:
    MOCK_METHOD(void *, memcpy, (const char *, const int, const char *, void *, const void *, size_t));
    MOCK_METHOD(void *, memmove, (const char *, const int, const char *, void *, const void *, size_t));
    MOCK_METHOD(void *, memset, (const char *, const int, const char *, void *, int, size_t));
    MOCK_METHOD(char *, strchr, (const char *, const int, const char *, const char *, int));
    MOCK_METHOD(int, strcmp, (const char *, const int, const char *, const char *, const char *));
    MOCK_METHOD(size_t, strlen, (const char *, const int, const char *, const char *));
    MOCK_METHOD(int, strncmp, (const char *, const int, const char *, const char *, const char *, size_t));
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
