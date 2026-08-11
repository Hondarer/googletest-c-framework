#include <test_com.h>
#include <mock_unistd.h>

#ifndef _WIN32

int delegate_real_pipe(const char *file, const int line, const char *func, int pipefd[2])
{
    (void)file;
    (void)line;
    (void)func;
    return pipe(pipefd);
}

int mock_pipe(const char *file, const int line, const char *func, int pipefd[2])
{
    return (_mock_unistd != nullptr) ? _mock_unistd->pipe(file, line, func, pipefd)
                                     : delegate_real_pipe(file, line, func, pipefd);
}

#endif // _WIN32
