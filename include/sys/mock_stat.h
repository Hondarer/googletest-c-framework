#ifndef _MOCK_SYS_STAT_H
#define _MOCK_SYS_STAT_H

#include <sys/stat.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern int mock_stat(const char *, const int, const char *, const char *, struct stat *);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STAT_H

#define stat(path, buf) mock_stat(__FILE__, __LINE__, __func__, path, buf)

#else // _IN_OVERRIDE_HEADER_STAT_H

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

extern int delegate_real_stat(const char *, const int, const char *, const char *, struct stat *);

class Mock_sys_stat
{
public:
MOCK_METHOD(int, stat, (const char *, const int, const char *, const char *, struct stat *));

    Mock_sys_stat();
    ~Mock_sys_stat();
};

extern Mock_sys_stat *_mock_sys_stat;

#endif // _IN_OVERRIDE_HEADER_STAT_H

#endif // _MOCK_SYS_STAT_H
