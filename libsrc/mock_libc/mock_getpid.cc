#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

pid_t delegate_real_getpid(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;
    return getpid();
}

pid_t mock_getpid(const char *file, const int line, const char *func)
{
    return (_mock_unistd != nullptr) ? _mock_unistd->getpid(file, line, func) : delegate_real_getpid(file, line, func);
}

#endif // _WIN32
