#ifndef _MOCK_DLFCN_H
#define _MOCK_DLFCN_H

#ifndef _WIN32

    /* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_DLFCN_H
        #include <dlfcn.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern void *mock_dlopen(const char *, const int, const char *, const char *, int);
    extern void *mock_dlsym(const char *, const int, const char *, void *, const char *);
    extern int mock_dlclose(const char *, const int, const char *, void *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_DLFCN_H

        #define dlopen(filename, flags) \
            mock_dlopen(__FILE__, __LINE__, __func__, filename, flags)
        #define dlsym(handle, symbol) \
            mock_dlsym(__FILE__, __LINE__, __func__, handle, symbol)
        #define dlclose(handle) \
            mock_dlclose(__FILE__, __LINE__, __func__, handle)

    #else // _IN_OVERRIDE_HEADER_DLFCN_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern void *delegate_real_dlopen(const char *, const int, const char *, const char *, int);
extern void *delegate_real_dlsym(const char *, const int, const char *, void *, const char *);
extern int delegate_real_dlclose(const char *, const int, const char *, void *);

class Mock_dlfcn
{
  public:
    MOCK_METHOD(void *, dlopen, (const char *, const int, const char *, const char *, int));
    MOCK_METHOD(void *, dlsym, (const char *, const int, const char *, void *, const char *));
    MOCK_METHOD(int, dlclose, (const char *, const int, const char *, void *));

    Mock_dlfcn();
    ~Mock_dlfcn();
};

extern Mock_dlfcn *_mock_dlfcn;

    #endif // _IN_OVERRIDE_HEADER_DLFCN_H

#endif // _WIN32

#endif // _MOCK_DLFCN_H
