#include <test_com.h>

char *testing::allocprintf(const char *fmt, ...)
{
    va_list args;
    char *str;

    va_start(args, fmt);
    str = allocvprintf(fmt, args);
    va_end(args);

    return str;
}

char *testing::allocvprintf(const char *fmt, va_list args)
{
    va_list args_copy;
    int len;
    size_t size;
    char *str;

    va_copy(args_copy, args);
    len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    // vsnprintf は符号化エラー時に負値を返す。負値のまま size_t へ変換すると
    // malloc へ巨大値が渡るため、ここで弾いてから変換する。
    if (len < 0)
    {
        return NULL;
    }

    size = (size_t)len + 1; // +1 for null terminator

    str = (char *)malloc(size);
    if (str == NULL)
    {
        return NULL;
    }

    va_copy(args_copy, args);
    vsnprintf(str, size, fmt, args_copy);
    va_end(args_copy);

    return str;
}