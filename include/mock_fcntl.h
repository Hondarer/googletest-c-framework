#ifndef _MOCK_FCNTL_H
#define _MOCK_FCNTL_H

#ifndef _WIN32

    #include <fcntl.h>
    #include <stdint.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_open(const char *, const int, const char *, const char *, int, int);
    extern int mock_fcntl(const char *, const int, const char *, int, int, intptr_t);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_FCNTL_H

        /* open も mode を省略できる可変長引数関数である。fcntl と同じ方式で      */
        /* 引数の個数によりマクロを選択し、省略時は mode に 0 を補う。            */
        #define _MOCK_OPEN_SELECT(_1, _2, _3, NAME, ...) NAME
        #define _MOCK_OPEN_2(path, flags)       mock_open(__FILE__, __LINE__, __func__, path, flags, 0)
        #define _MOCK_OPEN_3(path, flags, mode) mock_open(__FILE__, __LINE__, __func__, path, flags, mode)
        #define open(...)                       _MOCK_OPEN_SELECT(__VA_ARGS__, _MOCK_OPEN_3, _MOCK_OPEN_2)(__VA_ARGS__)

        /* fcntl は追加引数が 0 個または 1 個の可変長引数関数である。            */
        /* MOCK_METHOD は可変長引数を扱えないため、引数の個数でマクロを選択し、  */
        /* 追加引数を intptr_t 1 個へ畳んだ固定シグネチャの mock_fcntl へ渡す。  */
        /* 追加引数を取らない呼び出しでは 0 を補う。                             */
        #define _MOCK_FCNTL_SELECT(_1, _2, _3, NAME, ...) NAME
        #define _MOCK_FCNTL_2(fd, cmd)      mock_fcntl(__FILE__, __LINE__, __func__, fd, cmd, (intptr_t)0)
        #define _MOCK_FCNTL_3(fd, cmd, arg) mock_fcntl(__FILE__, __LINE__, __func__, fd, cmd, (intptr_t)(arg))
        #define fcntl(...)                  _MOCK_FCNTL_SELECT(__VA_ARGS__, _MOCK_FCNTL_3, _MOCK_FCNTL_2)(__VA_ARGS__)

    #else // _IN_OVERRIDE_HEADER_FCNTL_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_open(const char *, const int, const char *, const char *, int, int);
extern int delegate_real_fcntl(const char *, const int, const char *, int, int, intptr_t);

class Mock_fcntl
{
  public:
    MOCK_METHOD(int, open, (const char *, const int, const char *, const char *, int, int));
    MOCK_METHOD(int, fcntl, (const char *, const int, const char *, int, int, intptr_t));

    Mock_fcntl();
    ~Mock_fcntl();
};

extern Mock_fcntl *_mock_fcntl;

    #endif // _IN_OVERRIDE_HEADER_FCNTL_H

#endif // _WIN32

#endif // _MOCK_FCNTL_H
