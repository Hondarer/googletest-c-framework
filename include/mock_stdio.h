#ifndef _MOCK_STDIO_H
#define _MOCK_STDIO_H

#include <stdio.h>
#include <stdarg.h>
#include <format_attr.h>
#ifdef _WIN32
    #include <wchar.h>
#else
    #include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    extern int mock_fclose(const char *, const int, const char *, FILE *);
    extern int mock_feof(const char *, const int, const char *, FILE *);
    extern int mock_ferror(const char *, const int, const char *, FILE *);
    extern int mock_fflush(const char *, const int, const char *, FILE *);
    extern FILE *mock_fopen(const char *, const int, const char *, const char *, const char *);
#ifdef _WIN32
    extern errno_t mock_fopen_s(const char *, const int, const char *, FILE **, const char *, const char *);
    extern errno_t mock__wfopen_s(const char *, const int, const char *, FILE **, const wchar_t *, const wchar_t *);
    extern FILE *mock__wfsopen(const char *, const int, const char *, const wchar_t *, const wchar_t *, int);
#endif
    extern int mock_printf(PRINTF_FMT const char *, const int, const char *, const char *, ...) PRINTF_ATTR(4, 5);
    extern int mock_fprintf(PRINTF_FMT const char *, const int, const char *, FILE *, const char *, ...)
        PRINTF_ATTR(5, 6);
    extern int mock_vfprintf(PRINTF_FMT const char *, const int, const char *, FILE *, const char *, va_list)
        PRINTF_ATTR(5, 0);
    extern int mock_snprintf(const char *, const int, const char *, char *, size_t, const char *, ...)
        PRINTF_ATTR(6, 7);
    extern int mock_vsnprintf(const char *, const int, const char *, char *, size_t, PRINTF_FMT const char *, va_list)
        PRINTF_ATTR(6, 0);
    extern int mock_scanf(SCANF_FMT const char *, const int, const char *, const char *, ...) SCANF_ATTR(4, 5);
    extern int mock_vscanf(SCANF_FMT const char *, const int, const char *, const char *, va_list) SCANF_ATTR(4, 0);
    extern int mock_vfscanf(SCANF_FMT const char *, const int, const char *, FILE *, const char *, va_list)
        SCANF_ATTR(5, 0);
    extern char *mock_fgets(const char *, const int, const char *, char *, int, FILE *);
    extern size_t mock_fread(const char *, const int, const char *, void *, size_t, size_t, FILE *);
    extern size_t mock_fwrite(const char *, const int, const char *, const void *, size_t, size_t, FILE *);
    extern FILE *mock_freopen(const char *, const int, const char *, const char *, const char *, FILE *);
    extern int mock_remove(const char *, const int, const char *, const char *);
    extern int mock_rename(const char *, const int, const char *, const char *, const char *);
#ifndef _WIN32
    extern FILE *mock_fdopen(const char *, const int, const char *, int, const char *);
    extern int mock_fseeko(const char *, const int, const char *, FILE *, off_t, int);
    extern off_t mock_ftello(const char *, const int, const char *, FILE *);
#else
extern int mock__fseeki64(const char *, const int, const char *, FILE *, __int64, int);
extern __int64 mock__ftelli64(const char *, const int, const char *, FILE *);
#endif

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STDIO_H

    #define fclose(stream)         mock_fclose(__FILE__, __LINE__, __func__, stream)
    #define feof(stream)           mock_feof(__FILE__, __LINE__, __func__, stream)
    #define ferror(stream)         mock_ferror(__FILE__, __LINE__, __func__, stream)
    #define fflush(stream)         mock_fflush(__FILE__, __LINE__, __func__, stream)
    #define fopen(filename, modes) mock_fopen(__FILE__, __LINE__, __func__, filename, modes)
    #ifdef _WIN32
        #define fopen_s(pFile, filename, modes)   mock_fopen_s(__FILE__, __LINE__, __func__, pFile, filename, modes)
        #define _wfopen_s(pFile, filename, modes) mock__wfopen_s(__FILE__, __LINE__, __func__, pFile, filename, modes)
        #define _wfsopen(filename, modes, shflag) mock__wfsopen(__FILE__, __LINE__, __func__, filename, modes, shflag)
    #endif /* _WIN32 */
    #define printf(format, ...)              mock_printf(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
    #define fprintf(stream, format, ...)     mock_fprintf(__FILE__, __LINE__, __func__, stream, format, ##__VA_ARGS__)
    #define vfprintf(stream, format, ap)     mock_vfprintf(__FILE__, __LINE__, __func__, stream, format, ap)
    #define snprintf(s, n, format, ...)      mock_snprintf(__FILE__, __LINE__, __func__, s, n, format, ##__VA_ARGS__)
    #define vsnprintf(s, n, format, ap)      mock_vsnprintf(__FILE__, __LINE__, __func__, s, n, format, ap)
    #define scanf(format, ...)               mock_scanf(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
    #define vscanf(format, ap)               mock_vscanf(__FILE__, __LINE__, __func__, format, ap)
    #define vfscanf(stream, format, ap)      mock_vfscanf(__FILE__, __LINE__, __func__, stream, format, ap)
    #define fgets(s, n, stream)              mock_fgets(__FILE__, __LINE__, __func__, s, n, stream)
    #define fread(ptr, size, count, stream)  mock_fread(__FILE__, __LINE__, __func__, ptr, size, count, stream)
    #define fwrite(ptr, size, count, stream) mock_fwrite(__FILE__, __LINE__, __func__, ptr, size, count, stream)
    #define freopen(path, modes, stream)     mock_freopen(__FILE__, __LINE__, __func__, path, modes, stream)
    /* C++ 標準ライブラリの std::remove と衝突するため、C のテスト対象だけを置換する。 */
    #ifndef __cplusplus
        #define remove(path)             mock_remove(__FILE__, __LINE__, __func__, path)
        #define rename(oldpath, newpath) mock_rename(__FILE__, __LINE__, __func__, oldpath, newpath)
    #endif /* !__cplusplus */
    #ifndef _WIN32
        #define fdopen(fd, modes)              mock_fdopen(__FILE__, __LINE__, __func__, fd, modes)
        #define fseeko(stream, offset, whence) mock_fseeko(__FILE__, __LINE__, __func__, stream, offset, whence)
        #define ftello(stream)                 mock_ftello(__FILE__, __LINE__, __func__, stream)
    #else
        #define _fseeki64(stream, offset, whence) mock__fseeki64(__FILE__, __LINE__, __func__, stream, offset, whence)
        #define _ftelli64(stream)                 mock__ftelli64(__FILE__, __LINE__, __func__, stream)
    #endif

#else // _IN_OVERRIDE_HEADER_STDIO_H

    #ifndef _WIN32
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
    #endif // _WIN32
    #include <gmock/gmock.h>
    #ifndef _WIN32
        #pragma GCC diagnostic pop
    #endif // _WIN32

extern int delegate_real_fclose(const char *, const int, const char *, FILE *);
extern int delegate_fake_fclose(const char *, const int, const char *, FILE *);
extern int delegate_real_feof(const char *, const int, const char *, FILE *);
extern int delegate_fake_feof(const char *, const int, const char *, FILE *);
extern int delegate_real_ferror(const char *, const int, const char *, FILE *);
extern int delegate_fake_ferror(const char *, const int, const char *, FILE *);
extern int delegate_real_fflush(const char *, const int, const char *, FILE *);
extern int delegate_fake_fflush(const char *, const int, const char *, FILE *);
extern FILE *delegate_real_fopen(const char *, const int, const char *, const char *, const char *);
extern FILE *delegate_fake_fopen(const char *, const int, const char *, const char *, const char *);
extern void reset_fake_fopen();
    #ifdef _WIN32
extern errno_t delegate_real_fopen_s(const char *, const int, const char *, FILE **, const char *, const char *);
extern errno_t delegate_fake_fopen_s(const char *, const int, const char *, FILE **, const char *, const char *);
extern errno_t delegate_real__wfopen_s(const char *, const int, const char *, FILE **, const wchar_t *,
                                       const wchar_t *);
extern errno_t delegate_fake__wfopen_s(const char *, const int, const char *, FILE **, const wchar_t *,
                                       const wchar_t *);
extern FILE *delegate_real__wfsopen(const char *, const int, const char *, const wchar_t *, const wchar_t *, int);
extern FILE *delegate_fake__wfsopen(const char *, const int, const char *, const wchar_t *, const wchar_t *, int);
    #endif
extern int delegate_real_fprintf(const char *, const int, const char *, FILE *, const char *);
extern int delegate_fake_fprintf(const char *, const int, const char *, FILE *, const char *);
extern int delegate_real_vfprintf(const char *, const int, const char *, FILE *, const char *);
extern int delegate_fake_vfprintf(const char *, const int, const char *, FILE *, const char *);
extern int delegate_real_snprintf(const char *, const int, const char *, char *, size_t, const char *);
extern int delegate_real_vsnprintf(const char *, const int, const char *, char *, size_t, const char *);
extern int delegate_fake_vsnprintf(const char *, const int, const char *, char *, size_t, const char *);
extern char *delegate_real_fgets(const char *, const int, const char *, char *, int, FILE *);
extern char *delegate_fake_fgets(const char *, const int, const char *, char *, int, FILE *);
extern size_t delegate_real_fread(const char *, const int, const char *, void *, size_t, size_t, FILE *);
extern size_t delegate_fake_fread(const char *, const int, const char *, void *, size_t, size_t, FILE *);
extern size_t delegate_real_fwrite(const char *, const int, const char *, const void *, size_t, size_t, FILE *);
extern size_t delegate_fake_fwrite(const char *, const int, const char *, const void *, size_t, size_t, FILE *);

extern int delegate_real_printf(const char *, const int, const char *, const char *);
extern int delegate_real_scanf(const char *, const int, const char *, const char *, va_list) SCANF_ATTR(1, 0);
extern int delegate_real_vscanf(const char *, const int, const char *, const char *, va_list) SCANF_ATTR(1, 0);
extern int delegate_real_vfscanf(const char *, const int, const char *, FILE *, const char *, va_list) SCANF_ATTR(1, 0);
extern FILE *delegate_real_freopen(const char *, const int, const char *, const char *, const char *, FILE *);
extern FILE *delegate_fake_freopen(const char *, const int, const char *, const char *, const char *, FILE *);
extern int delegate_real_remove(const char *, const int, const char *, const char *);
extern int delegate_fake_remove(const char *, const int, const char *, const char *);
extern int delegate_real_rename(const char *, const int, const char *, const char *, const char *);
extern int delegate_fake_rename(const char *, const int, const char *, const char *, const char *);
    #ifndef _WIN32
extern FILE *delegate_real_fdopen(const char *, const int, const char *, int, const char *);
extern FILE *delegate_fake_fdopen(const char *, const int, const char *, int, const char *);
extern int delegate_real_fseeko(const char *, const int, const char *, FILE *, off_t, int);
extern int delegate_fake_fseeko(const char *, const int, const char *, FILE *, off_t, int);
extern off_t delegate_real_ftello(const char *, const int, const char *, FILE *);
extern off_t delegate_fake_ftello(const char *, const int, const char *, FILE *);
    #else
extern int delegate_real__fseeki64(const char *, const int, const char *, FILE *, __int64, int);
extern int delegate_fake__fseeki64(const char *, const int, const char *, FILE *, __int64, int);
extern __int64 delegate_real__ftelli64(const char *, const int, const char *, FILE *);
extern __int64 delegate_fake__ftelli64(const char *, const int, const char *, FILE *);
    #endif

class Mock_stdio
{
  public:
    MOCK_METHOD(int, access, (const char *, const int, const char *, const char *, int));
    MOCK_METHOD(int, fclose, (const char *, const int, const char *, FILE *));
    MOCK_METHOD(int, feof, (const char *, const int, const char *, FILE *));
    MOCK_METHOD(int, ferror, (const char *, const int, const char *, FILE *));
    MOCK_METHOD(int, fflush, (const char *, const int, const char *, FILE *));
    MOCK_METHOD(FILE *, fopen, (const char *, const int, const char *, const char *, const char *));
    #ifdef _WIN32
    MOCK_METHOD(errno_t, fopen_s, (const char *, const int, const char *, FILE **, const char *, const char *));
    MOCK_METHOD(errno_t, _wfopen_s, (const char *, const int, const char *, FILE **, const wchar_t *, const wchar_t *));
    MOCK_METHOD(FILE *, _wfsopen, (const char *, const int, const char *, const wchar_t *, const wchar_t *, int));
    #endif
    MOCK_METHOD(int, fprintf, (const char *, const int, const char *, FILE *, const char *));
    MOCK_METHOD(int, vfprintf, (const char *, const int, const char *, FILE *, const char *));
    MOCK_METHOD(int, snprintf, (const char *, const int, const char *, char *, size_t, const char *));
    MOCK_METHOD(int, vsnprintf, (const char *, const int, const char *, char *, size_t, const char *));
    MOCK_METHOD(char *, fgets, (const char *, const int, const char *, char *, int, FILE *));
    MOCK_METHOD(size_t, fread, (const char *, const int, const char *, void *, size_t, size_t, FILE *));
    MOCK_METHOD(size_t, fwrite, (const char *, const int, const char *, const void *, size_t, size_t, FILE *));

    void switch_to_real_fileio();
    void switch_to_mock_fileio();

    MOCK_METHOD(int, printf, (const char *, const int, const char *, const char *));
    MOCK_METHOD(int, scanf, (const char *, const int, const char *, const char *, va_list));
    MOCK_METHOD(int, vscanf, (const char *, const int, const char *, const char *, va_list));
    MOCK_METHOD(int, vfscanf, (const char *, const int, const char *, FILE *, const char *, va_list));
    MOCK_METHOD(FILE *, freopen, (const char *, const int, const char *, const char *, const char *, FILE *));
    MOCK_METHOD(int, remove, (const char *, const int, const char *, const char *));
    MOCK_METHOD(int, rename, (const char *, const int, const char *, const char *, const char *));
    #ifndef _WIN32
    MOCK_METHOD(FILE *, fdopen, (const char *, const int, const char *, int, const char *));
    MOCK_METHOD(int, fseeko, (const char *, const int, const char *, FILE *, off_t, int));
    MOCK_METHOD(off_t, ftello, (const char *, const int, const char *, FILE *));
    #else
    MOCK_METHOD(int, _fseeki64, (const char *, const int, const char *, FILE *, __int64, int));
    MOCK_METHOD(__int64, _ftelli64, (const char *, const int, const char *, FILE *));
    #endif

    Mock_stdio();
    ~Mock_stdio();
};

extern Mock_stdio *_mock_stdio;

#endif // _IN_OVERRIDE_HEADER_STDIO_H

#endif // _MOCK_STDIO_H
