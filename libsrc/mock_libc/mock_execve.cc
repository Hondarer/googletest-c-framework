#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

int delegate_real_execve(const char *file, const int line, const char *func, const char *path, char *const argv[],
                         char *const envp[])
{
    (void)file;
    (void)line;
    (void)func;
    return execve(path, argv, envp);
}

int mock_execve(const char *file, const int line, const char *func, const char *path, char *const argv[],
                char *const envp[])
{
    return (_mock_unistd != nullptr) ? _mock_unistd->execve(file, line, func, path, argv, envp)
                                     : delegate_real_execve(file, line, func, path, argv, envp);
}

#endif // _WIN32
