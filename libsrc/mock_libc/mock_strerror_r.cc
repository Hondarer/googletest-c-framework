#include <test_com.h>
#include <mock_string.h>

#ifndef _WIN32

    #include <errno.h>
    #include <string.h>

using namespace testing;

/* strerror_r には 2 系統がある。XSI 準拠版は int を返して buf へ書き込み、
 * GNU 拡張版は char * を返す (buf を使わず静的文字列を返すことがある)。
 * glibc は _GNU_SOURCE 定義時のみ GNU 版を提供する。
 * see: https://man7.org/linux/man-pages/man3/strerror_r.3.html
 *
 * 本モック ライブラリは C++ でビルドされ、libstdc++ が _GNU_SOURCE を定義するため
 * GNU 版が選択される。一方、テスト対象の C ソースは XSI 版を呼び出す。
 * 委譲先で GNU 版の結果を XSI 版の意味論へ変換して差を吸収する。 */
int delegate_real_strerror_r(const char *file, const int line, const char *func, int errnum, char *buf, size_t buflen)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (buf == nullptr || buflen == 0u)
    {
        return EINVAL;
    }

    #if defined(__GLIBC__) && defined(_GNU_SOURCE)
    {
        const char *message = strerror_r(errnum, buf, buflen);

        if (message == nullptr)
        {
            buf[0] = '\0';
            return EINVAL;
        }
        if (message != buf)
        {
            const size_t length = strlen(message);

            if (length + 1u > buflen)
            {
                memcpy(buf, message, buflen - 1u);
                buf[buflen - 1u] = '\0';
                return ERANGE;
            }
            memcpy(buf, message, length + 1u);
        }
        return 0;
    }
    #else  /* !(__GLIBC__ && _GNU_SOURCE) */
    return strerror_r(errnum, buf, buflen);
    #endif /* __GLIBC__ && _GNU_SOURCE */
}

int mock_strerror_r(const char *file, const int line, const char *func, int errnum, char *buf, size_t buflen)
{
    int result;

    if (_mock_string != nullptr)
    {
        result = _mock_string->strerror_r(file, line, func, errnum, buf, buflen);
    }
    else
    {
        result = delegate_real_strerror_r(file, line, func, errnum, buf, buflen);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > strerror_r %d", errnum);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
