#ifndef _MOCK_STDLIB_H
#define _MOCK_STDLIB_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern void *mock_calloc(const char *, const int, const char *, size_t, size_t);
    extern char *mock_getenv(const char *, const int, const char *, const char *);
#ifndef _WIN32
    /* setenv / unsetenv は POSIX のみ。Windows は _putenv_s を使うためモック対象外 */
    extern int mock_setenv(const char *, const int, const char *, const char *, const char *, int);
    extern int mock_unsetenv(const char *, const int, const char *, const char *);
#endif // _WIN32

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STDLIB_H

    #define calloc(__nmemb, __size) mock_calloc(__FILE__, __LINE__, __func__, __nmemb, __size)
    #define getenv(__name)          mock_getenv(__FILE__, __LINE__, __func__, __name)
    #ifndef _WIN32
        #define setenv(__name, __value, __overwrite) \
            mock_setenv(__FILE__, __LINE__, __func__, __name, __value, __overwrite)
        #define unsetenv(__name) mock_unsetenv(__FILE__, __LINE__, __func__, __name)
    #endif // _WIN32

#else // _IN_OVERRIDE_HEADER_STDLIB_H

    #ifndef _WIN32
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
    #endif // _WIN32
    #include <gmock/gmock.h>
    #ifndef _WIN32
        #pragma GCC diagnostic pop
    #endif // _WIN32

extern void *delegate_real_calloc(const char *, const int, const char *, size_t, size_t);
extern char *delegate_real_getenv(const char *, const int, const char *, const char *);
    #ifndef _WIN32
extern int delegate_real_setenv(const char *, const int, const char *, const char *, const char *, int);
extern int delegate_real_unsetenv(const char *, const int, const char *, const char *);
    #endif // _WIN32

class Mock_stdlib
{
  public:
    MOCK_METHOD(void *, calloc, (const char *, const int, const char *, size_t, size_t));
    MOCK_METHOD(char *, getenv, (const char *, const int, const char *, const char *));
    #ifndef _WIN32
    MOCK_METHOD(int, setenv, (const char *, const int, const char *, const char *, const char *, int));
    MOCK_METHOD(int, unsetenv, (const char *, const int, const char *, const char *));
    #endif // _WIN32

    Mock_stdlib();
    ~Mock_stdlib();
};

extern Mock_stdlib *_mock_stdlib;

#endif // _IN_OVERRIDE_HEADER_STDLIB_H

#endif // _MOCK_STDLIB_H
