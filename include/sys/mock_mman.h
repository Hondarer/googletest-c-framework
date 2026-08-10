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

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_MMAN_H

        #define mmap(addr, length, prot, flags, fd, offset) \
            mock_mmap(__FILE__, __LINE__, __func__, addr, length, prot, flags, fd, offset)
        #define munmap(addr, length) \
            mock_munmap(__FILE__, __LINE__, __func__, addr, length)
        #define msync(addr, length, flags) \
            mock_msync(__FILE__, __LINE__, __func__, addr, length, flags)

    #else // _IN_OVERRIDE_HEADER_SYS_MMAN_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern void *delegate_real_mmap(const char *, const int, const char *, void *, size_t, int, int, int, off_t);
extern int delegate_real_munmap(const char *, const int, const char *, void *, size_t);
extern int delegate_real_msync(const char *, const int, const char *, void *, size_t, int);

class Mock_sys_mman
{
  public:
    MOCK_METHOD(void *, mmap, (const char *, const int, const char *, void *, size_t, int, int, int, off_t));
    MOCK_METHOD(int, munmap, (const char *, const int, const char *, void *, size_t));
    MOCK_METHOD(int, msync, (const char *, const int, const char *, void *, size_t, int));

    Mock_sys_mman();
    ~Mock_sys_mman();
};

extern Mock_sys_mman *_mock_sys_mman;

    #endif // _IN_OVERRIDE_HEADER_SYS_MMAN_H

#endif // _WIN32

#endif // _MOCK_SYS_MMAN_H
