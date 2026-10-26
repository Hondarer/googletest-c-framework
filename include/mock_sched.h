#ifndef _MOCK_SCHED_H
#define _MOCK_SCHED_H

#ifndef _WIN32

    /* override 経由の場合、libc のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_SCHED_H
        #include <sched.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_sched_setscheduler(const char *, const int, const char *, pid_t, int, const struct sched_param *);
    extern int mock_sched_getscheduler(const char *, const int, const char *, pid_t);
    extern int mock_sched_getparam(const char *, const int, const char *, pid_t, struct sched_param *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SCHED_H

        #define sched_setscheduler(pid, policy, param) \
            mock_sched_setscheduler(__FILE__, __LINE__, __func__, pid, policy, param)
        #define sched_getscheduler(pid)       mock_sched_getscheduler(__FILE__, __LINE__, __func__, pid)
        #define sched_getparam(pid, param)    mock_sched_getparam(__FILE__, __LINE__, __func__, pid, param)

    #else // _IN_OVERRIDE_HEADER_SCHED_H

        #include <gmock/gmock.h>

extern int delegate_real_sched_setscheduler(const char *, const int, const char *, pid_t, int,
                                            const struct sched_param *);
extern int delegate_real_sched_getscheduler(const char *, const int, const char *, pid_t);
extern int delegate_real_sched_getparam(const char *, const int, const char *, pid_t, struct sched_param *);

class Mock_sched
{
  public:
    MOCK_METHOD(int, sched_setscheduler, (const char *, const int, const char *, pid_t, int, const struct sched_param *));
    MOCK_METHOD(int, sched_getscheduler, (const char *, const int, const char *, pid_t));
    MOCK_METHOD(int, sched_getparam, (const char *, const int, const char *, pid_t, struct sched_param *));

    Mock_sched();
    virtual ~Mock_sched();
};

extern Mock_sched *_mock_sched;

    #endif // _IN_OVERRIDE_HEADER_SCHED_H

#endif // _WIN32

#endif // _MOCK_SCHED_H
