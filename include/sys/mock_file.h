#ifndef _MOCK_SYS_FILE_H
#define _MOCK_SYS_FILE_H

#ifndef _WIN32

    #include <sys/file.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_flock(const char *, const int, const char *, int, int);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SYS_FILE_H

        #define flock(fd, operation) mock_flock(__FILE__, __LINE__, __func__, fd, operation)

    #else // _IN_OVERRIDE_HEADER_SYS_FILE_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_flock(const char *, const int, const char *, int, int);

class Mock_sys_file
{
  public:
    MOCK_METHOD(int, flock, (const char *, const int, const char *, int, int));

    Mock_sys_file();
    ~Mock_sys_file();
};

extern Mock_sys_file *_mock_sys_file;

    #endif // _IN_OVERRIDE_HEADER_SYS_FILE_H

#endif // _WIN32

#endif // _MOCK_SYS_FILE_H
