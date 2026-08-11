#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

uid_t delegate_real_geteuid(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;
    return geteuid();
}

uid_t mock_geteuid(const char *file, const int line, const char *func)
{
    return (_mock_unistd != nullptr) ? _mock_unistd->geteuid(file, line, func)
                                     : delegate_real_geteuid(file, line, func);
}

#endif // _WIN32
