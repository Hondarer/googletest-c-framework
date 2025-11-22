#ifndef _MOCK_STDLIB_H
#define _MOCK_STDLIB_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern void *mock_calloc(const char *, const int, const char *, size_t, size_t);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STDLIB_H

#define calloc(__nmemb, __size) mock_calloc(__FILE__, __LINE__, __func__, __nmemb, __size)

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

class Mock_stdlib
{
public:
    MOCK_METHOD(void *, calloc, (const char *, const int, const char *, size_t, size_t));

    Mock_stdlib();
    ~Mock_stdlib();
};

extern Mock_stdlib *_mock_stdlib;

#endif // _IN_OVERRIDE_HEADER_STDLIB_H

#endif // _MOCK_STDLIB_H
