#ifndef _MOCK_FCNTL_H
#define _MOCK_FCNTL_H

#ifndef _WIN32

    #include <fcntl.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_open(const char *, const int, const char *, const char *, int, int);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_FCNTL_H

        #define open(path, flags, mode) mock_open(__FILE__, __LINE__, __func__, path, flags, mode)

    #else // _IN_OVERRIDE_HEADER_FCNTL_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_open(const char *, const int, const char *, const char *, int, int);

class Mock_fcntl
{
  public:
    MOCK_METHOD(int, open, (const char *, const int, const char *, const char *, int, int));

    Mock_fcntl();
    ~Mock_fcntl();
};

extern Mock_fcntl *_mock_fcntl;

    #endif // _IN_OVERRIDE_HEADER_FCNTL_H

#endif // _WIN32

#endif // _MOCK_FCNTL_H
