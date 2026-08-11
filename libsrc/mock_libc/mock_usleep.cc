#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

int delegate_real_usleep(const char *file, const int line, const char *func, useconds_t usec)
{
    (void)file;
    (void)line;
    (void)func;
    return usleep(usec);
}

int mock_usleep(const char *file, const int line, const char *func, useconds_t usec)
{
    return (_mock_unistd != nullptr) ? _mock_unistd->usleep(file, line, func, usec)
                                     : delegate_real_usleep(file, line, func, usec);
}

#endif // _WIN32
