#ifndef _MOCK_SYS_STAT_H
#define _MOCK_SYS_STAT_H

#include <sys/stat.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern int mock_stat(const char *, const int, const char *, const char *, struct stat *);
#ifndef _WIN32
    /* fstat は POSIX のみ。Windows は GetFileInformationByHandle を使うためモック対象外 */
    extern int mock_fstat(const char *, const int, const char *, int, struct stat *);
    extern int mock_mkdir(const char *, const int, const char *, const char *, mode_t);
    /* futimens / utimensat は POSIX.1-2008 のみ。Windows は SetFileTime を使うためモック対象外 */
    extern int mock_futimens(const char *, const int, const char *, int, const struct timespec *);
    extern int mock_utimensat(const char *, const int, const char *, int, const char *, const struct timespec *, int);
#endif // _WIN32

#ifdef _WIN32
    extern int mock_stat64(const char *, const int, const char *, const char *, struct _stat64 *);
#endif

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STAT_H

    #define stat(path, buf) mock_stat(__FILE__, __LINE__, __func__, path, buf)

    #ifndef _WIN32
        #define fstat(fd, buf)      mock_fstat(__FILE__, __LINE__, __func__, fd, buf)
        #define mkdir(path, mode)   mock_mkdir(__FILE__, __LINE__, __func__, path, mode)
        #define futimens(fd, times) mock_futimens(__FILE__, __LINE__, __func__, fd, times)
        #define utimensat(dirfd, path, times, flags) \
            mock_utimensat(__FILE__, __LINE__, __func__, dirfd, path, times, flags)
    #endif // _WIN32

    #ifdef _WIN32
        #define _stat64(path, buf) mock_stat64(__FILE__, __LINE__, __func__, path, buf)
    #endif

#else // _IN_OVERRIDE_HEADER_STAT_H

    #include <gmock/gmock.h>

extern int delegate_real_stat(const char *, const int, const char *, const char *, struct stat *);

    #ifndef _WIN32
extern int delegate_real_fstat(const char *, const int, const char *, int, struct stat *);
extern int delegate_real_mkdir(const char *, const int, const char *, const char *, mode_t);
extern int delegate_real_futimens(const char *, const int, const char *, int, const struct timespec *);
extern int delegate_real_utimensat(const char *, const int, const char *, int, const char *, const struct timespec *,
                                   int);
    #endif // _WIN32

    #ifdef _WIN32
extern int delegate_real_stat64(const char *, const int, const char *, const char *, struct _stat64 *);
    #endif

class Mock_sys_stat
{
  public:
    MOCK_METHOD(int, stat, (const char *, const int, const char *, const char *, struct stat *));

    #ifndef _WIN32
    MOCK_METHOD(int, fstat, (const char *, const int, const char *, int, struct stat *));
    MOCK_METHOD(int, mkdir, (const char *, const int, const char *, const char *, mode_t));
    MOCK_METHOD(int, futimens, (const char *, const int, const char *, int, const struct timespec *));
    MOCK_METHOD(int, utimensat,
                (const char *, const int, const char *, int, const char *, const struct timespec *, int));
    #endif // _WIN32

    #ifdef _WIN32
    MOCK_METHOD(int, stat64, (const char *, const int, const char *, const char *, struct _stat64 *));
    #endif

    Mock_sys_stat();
    ~Mock_sys_stat();
};

extern Mock_sys_stat *_mock_sys_stat;

#endif // _IN_OVERRIDE_HEADER_STAT_H

#endif // _MOCK_SYS_STAT_H
