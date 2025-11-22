#ifndef _WIN32

#ifndef _MOCK_WAIT_H
#define _MOCK_WAIT_H

#include <sys/types.h>
#include <sys/wait.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern pid_t mock_waitpid(const char *, const int, const char *, pid_t, int *, int);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_WAIT_H

#define waitpid(pid, stat_loc, options) mock_waitpid(__FILE__, __LINE__, __func__, pid, stat_loc, options)

#else // _IN_OVERRIDE_HEADER_WAIT_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#include <gmock/gmock.h>
#pragma GCC diagnostic pop

extern pid_t delegate_real_waitpid(const char *, const int, const char *, pid_t, int *, int);

class Mock_sys_wait
{
public:
    MOCK_METHOD(pid_t, waitpid, (const char *, const int, const char *, pid_t, int *, int));

    Mock_sys_wait();
    ~Mock_sys_wait();
};

extern Mock_sys_wait *_mock_sys_wait;

#endif // _IN_OVERRIDE_HEADER_WAIT_H

#endif // _MOCK_WAIT_H

#endif // _WIN32
