#ifndef _MOCK_UNISTD_H
#define _MOCK_UNISTD_H

#ifndef _WIN32
    #include <sys/types.h>
    #include <unistd.h>
#else // _WIN32
    #include <io.h>
#endif // _WIN32

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _WIN32
    extern int mock_access(const char *, const int, const char *, const char *, int);
    extern char *mock_getcwd(const char *, const int, const char *, char *, size_t);
    extern int mock_ftruncate(const char *, const int, const char *, int, off_t);
    extern pid_t mock_fork(const char *, const int, const char *);
    extern int mock_mkstemp(const char *, const int, const char *, char *);
    extern off_t mock_lseek(const char *, const int, const char *, int, off_t, int);
    extern int mock_close(const char *, const int, const char *, int);
    extern int mock_dup(const char *, const int, const char *, int);
    extern int mock_dup2(const char *, const int, const char *, int, int);
    extern ssize_t mock_read(const char *, const int, const char *, int, void *, size_t);
    extern ssize_t mock_write(const char *, const int, const char *, int, const void *, size_t);
    extern uid_t mock_geteuid(const char *, const int, const char *);
    extern pid_t mock_getpid(const char *, const int, const char *);
    extern int mock_kill(const char *, const int, const char *, pid_t, int);
    extern int mock_pipe(const char *, const int, const char *, int[2]);
    extern int mock_execve(const char *, const int, const char *, const char *, char *const[], char *const[]);
    extern ssize_t mock_readlink(const char *, const int, const char *, const char *, char *, size_t);
    extern int mock_usleep(const char *, const int, const char *, useconds_t);
    extern int mock_sched_yield(const char *, const int, const char *);
#else  // _WIN32
extern __int64 mock__lseeki64(const char *, const int, const char *, int, __int64, int);
extern int mock__close(const char *, const int, const char *, int);
extern int mock__dup(const char *, const int, const char *, int);
extern int mock__dup2(const char *, const int, const char *, int, int);
extern int mock__read(const char *, const int, const char *, int, void *, unsigned int);
extern int mock__write(const char *, const int, const char *, int, const void *, unsigned int);
#endif // _WIN32

#ifdef __cplusplus
}
#endif

#if defined(_IN_OVERRIDE_HEADER_UNISTD_H) || defined(_IN_OVERRIDE_HEADER_IO_H)

    #ifndef _WIN32

        #define access(path, amode)       mock_access(__FILE__, __LINE__, __func__, path, amode)
        #define getcwd(buf, size)         mock_getcwd(__FILE__, __LINE__, __func__, buf, size)
        #define ftruncate(fd, length)     mock_ftruncate(__FILE__, __LINE__, __func__, fd, length)
        #define fork()                    mock_fork(__FILE__, __LINE__, __func__)
        #define mkstemp(tmpl)             mock_mkstemp(__FILE__, __LINE__, __func__, tmpl)
        #define lseek(fd, offset, whence) mock_lseek(__FILE__, __LINE__, __func__, fd, offset, whence)
        #define close(fd)                 mock_close(__FILE__, __LINE__, __func__, fd)
        #define dup(fd)                   mock_dup(__FILE__, __LINE__, __func__, fd)
        #define dup2(oldfd, newfd)        mock_dup2(__FILE__, __LINE__, __func__, oldfd, newfd)
        #define read(fd, buf, count)      mock_read(__FILE__, __LINE__, __func__, fd, buf, count)
        #define write(fd, buf, count)     mock_write(__FILE__, __LINE__, __func__, fd, buf, count)
        #define geteuid()                 mock_geteuid(__FILE__, __LINE__, __func__)
        #define getpid()                  mock_getpid(__FILE__, __LINE__, __func__)
        #define kill(pid, signal)         mock_kill(__FILE__, __LINE__, __func__, pid, signal)
        #define pipe(pipefd)              mock_pipe(__FILE__, __LINE__, __func__, pipefd)
        #define execve(path, argv, envp)  mock_execve(__FILE__, __LINE__, __func__, path, argv, envp)
        #define readlink(path, buf, size) mock_readlink(__FILE__, __LINE__, __func__, path, buf, size)
        #define usleep(usec)              mock_usleep(__FILE__, __LINE__, __func__, usec)
        #define sched_yield()             mock_sched_yield(__FILE__, __LINE__, __func__)

    #else // _WIN32

        #define _lseeki64(fd, offset, whence) mock__lseeki64(__FILE__, __LINE__, __func__, fd, offset, whence)
        #define _close(fd)                    mock__close(__FILE__, __LINE__, __func__, fd)
        #define _dup(fd)                      mock__dup(__FILE__, __LINE__, __func__, fd)
        #define _dup2(oldfd, newfd)           mock__dup2(__FILE__, __LINE__, __func__, oldfd, newfd)
        #define _read(fd, buf, count)         mock__read(__FILE__, __LINE__, __func__, fd, buf, count)
        #define _write(fd, buf, count)        mock__write(__FILE__, __LINE__, __func__, fd, buf, count)

    #endif // _WIN32

#else // _IN_OVERRIDE_HEADER_UNISTD_H || _IN_OVERRIDE_HEADER_IO_H

    #ifndef _WIN32
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
    #endif // _WIN32
    #include <gmock/gmock.h>
    #ifndef _WIN32
        #pragma GCC diagnostic pop
    #endif // _WIN32

    #ifndef _WIN32
extern int delegate_real_access(const char *, const int, const char *, const char *, int);
extern char *delegate_real_getcwd(const char *, const int, const char *, char *, size_t);
extern int delegate_real_ftruncate(const char *, const int, const char *, int, off_t);
extern pid_t delegate_real_fork(const char *, const int, const char *);
extern int delegate_real_mkstemp(const char *, const int, const char *, char *);
extern off_t delegate_real_lseek(const char *, const int, const char *, int, off_t, int);
extern int delegate_real_close(const char *, const int, const char *, int);
extern int delegate_real_dup(const char *, const int, const char *, int);
extern int delegate_real_dup2(const char *, const int, const char *, int, int);
extern ssize_t delegate_real_read(const char *, const int, const char *, int, void *, size_t);
extern ssize_t delegate_real_write(const char *, const int, const char *, int, const void *, size_t);
extern uid_t delegate_real_geteuid(const char *, const int, const char *);
extern pid_t delegate_real_getpid(const char *, const int, const char *);
extern int delegate_real_kill(const char *, const int, const char *, pid_t, int);
extern int delegate_real_pipe(const char *, const int, const char *, int[2]);
extern int delegate_real_execve(const char *, const int, const char *, const char *, char *const[], char *const[]);
extern ssize_t delegate_real_readlink(const char *, const int, const char *, const char *, char *, size_t);
extern int delegate_real_usleep(const char *, const int, const char *, useconds_t);
extern int delegate_real_sched_yield(const char *, const int, const char *);

class Mock_unistd
{
  public:
    MOCK_METHOD(int, access, (const char *, const int, const char *, const char *, int));
    MOCK_METHOD(char *, getcwd, (const char *, const int, const char *, char *, size_t));
    MOCK_METHOD(int, ftruncate, (const char *, const int, const char *, int, off_t));
    MOCK_METHOD(pid_t, fork, (const char *, const int, const char *));
    MOCK_METHOD(int, mkstemp, (const char *, const int, const char *, char *));
    MOCK_METHOD(off_t, lseek, (const char *, const int, const char *, int, off_t, int));
    MOCK_METHOD(int, close, (const char *, const int, const char *, int));
    MOCK_METHOD(int, dup, (const char *, const int, const char *, int));
    MOCK_METHOD(int, dup2, (const char *, const int, const char *, int, int));
    MOCK_METHOD(ssize_t, read, (const char *, const int, const char *, int, void *, size_t));
    MOCK_METHOD(ssize_t, write, (const char *, const int, const char *, int, const void *, size_t));
    MOCK_METHOD(uid_t, geteuid, (const char *, const int, const char *));
    MOCK_METHOD(pid_t, getpid, (const char *, const int, const char *));
    MOCK_METHOD(int, kill, (const char *, const int, const char *, pid_t, int));
    MOCK_METHOD(int, pipe, (const char *, const int, const char *, int[2]));
    MOCK_METHOD(int, execve, (const char *, const int, const char *, const char *, char *const[], char *const[]));
    MOCK_METHOD(ssize_t, readlink, (const char *, const int, const char *, const char *, char *, size_t));
    MOCK_METHOD(int, usleep, (const char *, const int, const char *, useconds_t));
    MOCK_METHOD(int, sched_yield, (const char *, const int, const char *));

    Mock_unistd();
    ~Mock_unistd();
};
    #else  // _WIN32
extern __int64 delegate_real__lseeki64(const char *, const int, const char *, int, __int64, int);
extern int delegate_real__close(const char *, const int, const char *, int);
extern int delegate_real__dup(const char *, const int, const char *, int);
extern int delegate_real__dup2(const char *, const int, const char *, int, int);
extern int delegate_real__read(const char *, const int, const char *, int, void *, unsigned int);
extern int delegate_real__write(const char *, const int, const char *, int, const void *, unsigned int);

class Mock_unistd
{
  public:
    MOCK_METHOD(__int64, _lseeki64, (const char *, const int, const char *, int, __int64, int));
    MOCK_METHOD(int, _close, (const char *, const int, const char *, int));
    MOCK_METHOD(int, _dup, (const char *, const int, const char *, int));
    MOCK_METHOD(int, _dup2, (const char *, const int, const char *, int, int));
    MOCK_METHOD(int, _read, (const char *, const int, const char *, int, void *, unsigned int));
    MOCK_METHOD(int, _write, (const char *, const int, const char *, int, const void *, unsigned int));

    Mock_unistd();
    ~Mock_unistd();
};
    #endif // _WIN32

extern Mock_unistd *_mock_unistd;

#endif // _IN_OVERRIDE_HEADER_UNISTD_H || _IN_OVERRIDE_HEADER_IO_H

#endif // _MOCK_UNISTD_H
