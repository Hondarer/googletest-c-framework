#ifndef _MOCK_SYS_RESOURCE_H
#define _MOCK_SYS_RESOURCE_H

#ifndef _WIN32

    /* override 経由の場合、libc のヘッダーは include_override 側で取り込み済みである */
    #ifndef _IN_OVERRIDE_HEADER_SYS_RESOURCE_H
        #include <sys/resource.h>
    #endif

    #ifdef __cplusplus
extern "C"
{
    #endif

    /* glibc は which を _GNU_SOURCE の有無で enum または int として宣言する。  */
    /* C と C++ の双方から同じシグネチャで呼べるよう、mock は int で受け取る。 */
    extern int mock_setpriority(const char *, const int, const char *, int, id_t, int);
    extern int mock_getpriority(const char *, const int, const char *, int, id_t);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_RESOURCE_H

        #define setpriority(which, who, prio) \
            mock_setpriority(__FILE__, __LINE__, __func__, (int)(which), who, prio)
        #define getpriority(which, who) mock_getpriority(__FILE__, __LINE__, __func__, (int)(which), who)

    #else // _IN_OVERRIDE_HEADER_SYS_RESOURCE_H

        #include <gmock/gmock.h>

extern int delegate_real_setpriority(const char *, const int, const char *, int, id_t, int);
extern int delegate_real_getpriority(const char *, const int, const char *, int, id_t);

class Mock_sys_resource
{
  public:
    MOCK_METHOD(int, setpriority, (const char *, const int, const char *, int, id_t, int));
    MOCK_METHOD(int, getpriority, (const char *, const int, const char *, int, id_t));

    Mock_sys_resource();
    virtual ~Mock_sys_resource();
};

extern Mock_sys_resource *_mock_sys_resource;

    #endif // _IN_OVERRIDE_HEADER_SYS_RESOURCE_H

#endif // _WIN32

#endif // _MOCK_SYS_RESOURCE_H
