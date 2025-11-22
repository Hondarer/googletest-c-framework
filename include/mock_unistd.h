#ifndef _WIN32

#ifndef _MOCK_UNISTD_H
#define _MOCK_UNISTD_H

#include <sys/types.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern int mock_access(const char *, const int, const char *, const char *, int);
    extern pid_t mock_fork(const char *, const int, const char *);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_UNISTD_H

#define access(path, amode) mock_access(__FILE__, __LINE__, __func__, path, amode)
#define fork() mock_fork(__FILE__, __LINE__, __func__)

#else // _IN_OVERRIDE_HEADER_UNISTD_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#include <gmock/gmock.h>
#pragma GCC diagnostic pop

extern int delegate_real_access(const char *, const int, const char *, const char *, int);
extern pid_t delegate_real_fork(const char *, const int, const char *);

class Mock_unistd
{
public:
    MOCK_METHOD(int, access, (const char *, const int, const char *, const char *, int));
    MOCK_METHOD(pid_t, fork, (const char *, const int, const char *));

    Mock_unistd();
    ~Mock_unistd();
};

extern Mock_unistd *_mock_unistd;

#endif // _IN_OVERRIDE_HEADER_UNISTD_H

#endif // _MOCK_UNISTD_H

#endif // _WIN32
