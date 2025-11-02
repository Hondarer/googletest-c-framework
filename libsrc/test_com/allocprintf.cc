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
    int size;
    char *str;

    va_copy(args_copy, args);
    size = vsnprintf(NULL, 0, fmt, args_copy) + 1; // +1 for null terminator
    va_end(args_copy);

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