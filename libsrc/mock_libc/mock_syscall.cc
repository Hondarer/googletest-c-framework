#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

using namespace testing;

/* カーネルはシステム コールごとに必要な個数の引数だけを読むため、
   省略された追加引数に補った 0 をそのまま渡しても挙動は変わらない。
   see: https://man7.org/linux/man-pages/man2/syscall.2.html */
long delegate_real_syscall(const char *file, const int line, const char *func, long number, intptr_t arg1,
                           intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return syscall(number, arg1, arg2, arg3, arg4, arg5);
}

long mock_syscall(const char *file, const int line, const char *func, long number, intptr_t arg1, intptr_t arg2,
                  intptr_t arg3, intptr_t arg4, intptr_t arg5)
{
    long mock_ret;

    if (_mock_unistd != nullptr)
    {
        mock_ret = _mock_unistd->syscall(file, line, func, number, arg1, arg2, arg3, arg4, arg5);
    }
    else
    {
        mock_ret = delegate_real_syscall(file, line, func, number, arg1, arg2, arg3, arg4, arg5);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > syscall %ld", number);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %ld\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
