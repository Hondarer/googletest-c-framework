#include <test_com.h>
#include <mock_stdio.h>

using namespace testing;

FILE *delegate_fake_freopen(const char *file, const int line, const char *func, const char *path, const char *modes,
                            FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;
    (void)path;
    (void)modes;

    return stream;
}

FILE *delegate_real_freopen(const char *file, const int line, const char *func, const char *path, const char *modes,
                            FILE *stream)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef _WIN32
    /* 元 API の戻り値規約を保持するため freopen_s へ置き換えず、MSVC の非推奨警告だけを抑制する。
     * see: https://learn.microsoft.com/cpp/c-runtime-library/reference/freopen-wfreopen */
    #pragma warning(push)
    #pragma warning(disable : 4996)
#endif /* _WIN32 */
    FILE *reopened_stream = ::freopen(path, modes, stream);
#ifdef _WIN32
    #pragma warning(pop)
#endif /* _WIN32 */

    return reopened_stream;
}

FILE *mock_freopen(const char *file, const int line, const char *func, const char *path, const char *modes,
                   FILE *stream)
{
    FILE *mock_ret;

    if (_mock_stdio != nullptr)
    {
        mock_ret = _mock_stdio->freopen(file, line, func, path, modes, stream);
    }
    else
    {
        mock_ret = delegate_real_freopen(file, line, func, path, modes, stream);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > freopen %s, %s, 0x%p", path, modes, (void *)stream);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
