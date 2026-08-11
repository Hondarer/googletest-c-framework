#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

ssize_t delegate_real_readlink(const char *file, const int line, const char *func, const char *path, char *buf,
                               size_t size)
{
    (void)file;
    (void)line;
    (void)func;
    return readlink(path, buf, size);
}

ssize_t mock_readlink(const char *file, const int line, const char *func, const char *path, char *buf, size_t size)
{
    return (_mock_unistd != nullptr) ? _mock_unistd->readlink(file, line, func, path, buf, size)
                                     : delegate_real_readlink(file, line, func, path, buf, size);
}

#endif // _WIN32
