#ifndef _MOCK_TIME_H
#define _MOCK_TIME_H

#include <time.h>
#ifdef _WIN32
#include <errno.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _WIN32
    extern int mock_clock_gettime(const char *, const int, const char *, clockid_t, struct timespec *);
    extern struct tm *mock_gmtime_r(const char *, const int, const char *, const time_t *, struct tm *);
#else
    extern errno_t mock_gmtime_s(const char *, const int, const char *, struct tm *, const time_t *);
#endif

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_TIME_H

#ifndef _WIN32
#define clock_gettime(clk_id, tp) mock_clock_gettime(__FILE__, __LINE__, __func__, clk_id, tp)
#define gmtime_r(timep, result) mock_gmtime_r(__FILE__, __LINE__, __func__, timep, result)
#else
#define gmtime_s(utc_tm, timep) mock_gmtime_s(__FILE__, __LINE__, __func__, utc_tm, timep)
#endif

#else // _IN_OVERRIDE_HEADER_TIME_H

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#ifndef _WIN32
extern int delegate_real_clock_gettime(const char *, const int, const char *, clockid_t, struct timespec *);
extern int delegate_fake_clock_gettime(const char *, const int, const char *, clockid_t, struct timespec *);
extern struct tm *delegate_real_gmtime_r(const char *, const int, const char *, const time_t *, struct tm *);
extern struct tm *delegate_fake_gmtime_r(const char *, const int, const char *, const time_t *, struct tm *);

class Mock_time
{
public:
    MOCK_METHOD(int, clock_gettime, (const char *, const int, const char *, clockid_t, struct timespec *));
    MOCK_METHOD(struct tm *, gmtime_r, (const char *, const int, const char *, const time_t *, struct tm *));

    void switch_to_real_time();
    void switch_to_mock_time();

    Mock_time();
    ~Mock_time();
};
#else
extern errno_t delegate_real_gmtime_s(const char *, const int, const char *, struct tm *, const time_t *);
extern errno_t delegate_fake_gmtime_s(const char *, const int, const char *, struct tm *, const time_t *);

class Mock_time
{
public:
    MOCK_METHOD(errno_t, gmtime_s, (const char *, const int, const char *, struct tm *, const time_t *));

    void switch_to_real_time();
    void switch_to_mock_time();

    Mock_time();
    ~Mock_time();
};
#endif

extern Mock_time *_mock_time;

#endif // _IN_OVERRIDE_HEADER_TIME_H

#endif // _MOCK_TIME_H
