#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

int delegate_real_sched_yield(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;
    return sched_yield();
}

int mock_sched_yield(const char *file, const int line, const char *func)
{
    return (_mock_unistd != nullptr) ? _mock_unistd->sched_yield(file, line, func)
                                     : delegate_real_sched_yield(file, line, func);
}

#endif // _WIN32
