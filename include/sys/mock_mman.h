#ifndef _MOCK_SYS_MMAN_H
#define _MOCK_SYS_MMAN_H

#ifndef _WIN32

    /* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_SYS_MMAN_H
        #include <sys/mman.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern void *mock_mmap(const char *, const int, const char *, void *, size_t, int, int, int, off_t);
    extern int mock_munmap(const char *, const int, const char *, void *, size_t);
    extern int mock_msync(const char *, const int, const char *, void *, size_t, int);
    extern int mock_mlock(const char *, const int, const char *, const void *, size_t);
    extern int mock_munlock(const char *, const int, const char *, const void *, size_t);
    extern int mock_mlockall(const char *, const int, const char *, int);
    extern int mock_munlockall(const char *, const int, const char *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_MMAN_H

        #define mmap(addr, length, prot, flags, fd, offset) \
            mock_mmap(__FILE__, __LINE__, __func__, addr, length, prot, flags, fd, offset)
        #define munmap(addr, length)       mock_munmap(__FILE__, __LINE__, __func__, addr, length)
        #define msync(addr, length, flags) mock_msync(__FILE__, __LINE__, __func__, addr, length, flags)
        #define mlock(addr, length)        mock_mlock(__FILE__, __LINE__, __func__, addr, length)
        #define munlock(addr, length)      mock_munlock(__FILE__, __LINE__, __func__, addr, length)
        #define mlockall(flags)            mock_mlockall(__FILE__, __LINE__, __func__, flags)
        #define munlockall()               mock_munlockall(__FILE__, __LINE__, __func__)

    #else // _IN_OVERRIDE_HEADER_SYS_MMAN_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern void *delegate_real_mmap(const char *, const int, const char *, void *, size_t, int, int, int, off_t);
extern int delegate_real_munmap(const char *, const int, const char *, void *, size_t);
extern int delegate_real_msync(const char *, const int, const char *, void *, size_t, int);
extern int delegate_real_mlock(const char *, const int, const char *, const void *, size_t);
extern int delegate_real_munlock(const char *, const int, const char *, const void *, size_t);
extern int delegate_real_mlockall(const char *, const int, const char *, int);
extern int delegate_real_munlockall(const char *, const int, const char *);

class Mock_sys_mman
{
  public:
    MOCK_METHOD(void *, mmap, (const char *, const int, const char *, void *, size_t, int, int, int, off_t));
    MOCK_METHOD(int, munmap, (const char *, const int, const char *, void *, size_t));
    MOCK_METHOD(int, msync, (const char *, const int, const char *, void *, size_t, int));
    MOCK_METHOD(int, mlock, (const char *, const int, const char *, const void *, size_t));
    MOCK_METHOD(int, munlock, (const char *, const int, const char *, const void *, size_t));
    MOCK_METHOD(int, mlockall, (const char *, const int, const char *, int));
    MOCK_METHOD(int, munlockall, (const char *, const int, const char *));

    Mock_sys_mman();
    ~Mock_sys_mman();
};

extern Mock_sys_mman *_mock_sys_mman;

    #endif // _IN_OVERRIDE_HEADER_SYS_MMAN_H

#endif // _WIN32

#endif // _MOCK_SYS_MMAN_H
