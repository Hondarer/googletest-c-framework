#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

int delegate_real_kill(const char *file, const int line, const char *func, pid_t pid, int signal)
{
    (void)file;
    (void)line;
    (void)func;
    return kill(pid, signal);
}

int mock_kill(const char *file, const int line, const char *func, pid_t pid, int signal)
{
    return (_mock_unistd != nullptr) ? _mock_unistd->kill(file, line, func, pid, signal)
                                     : delegate_real_kill(file, line, func, pid, signal);
}

#endif // _WIN32
