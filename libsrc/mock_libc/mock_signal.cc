#include <mock_instance.h>
#include <test_com.h>
#include <mock_signal.h>

#ifndef _WIN32

using namespace testing;

Mock_signal *_mock_signal = nullptr;

Mock_signal::Mock_signal()
{
    ON_CALL(*this, sigaction(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sigaction));
    ON_CALL(*this, sigemptyset(_, _, _, _)).WillByDefault(Invoke(delegate_real_sigemptyset));
    ON_CALL(*this, sigaddset(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_sigaddset));
    ON_CALL(*this, pthread_sigmask(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_sigmask));
    ON_CALL(*this, signal(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_signal));
    ON_CALL(*this, raise(_, _, _, _)).WillByDefault(Invoke(delegate_real_raise));
    TESTFW_REGISTER_MOCK_INSTANCE(_mock_signal);
}

Mock_signal::~Mock_signal()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_signal);
}

int delegate_real_sigaction(const char *file, const int line, const char *func, int signum, const struct sigaction *act,
                            struct sigaction *oldact)
{
    (void)file;
    (void)line;
    (void)func;
    return sigaction(signum, act, oldact);
}

int mock_sigaction(const char *file, const int line, const char *func, int signum, const struct sigaction *act,
                   struct sigaction *oldact)
{
    int mock_ret = (_mock_signal != nullptr) ? _mock_signal->sigaction(file, line, func, signum, act, oldact)
                                             : delegate_real_sigaction(file, line, func, signum, act, oldact);
    return mock_ret;
}

int delegate_real_sigemptyset(const char *file, const int line, const char *func, sigset_t *set)
{
    (void)file;
    (void)line;
    (void)func;
    return sigemptyset(set);
}

int mock_sigemptyset(const char *file, const int line, const char *func, sigset_t *set)
{
    return (_mock_signal != nullptr) ? _mock_signal->sigemptyset(file, line, func, set)
                                     : delegate_real_sigemptyset(file, line, func, set);
}

int delegate_real_sigaddset(const char *file, const int line, const char *func, sigset_t *set, int signum)
{
    (void)file;
    (void)line;
    (void)func;
    return sigaddset(set, signum);
}

int mock_sigaddset(const char *file, const int line, const char *func, sigset_t *set, int signum)
{
    return (_mock_signal != nullptr) ? _mock_signal->sigaddset(file, line, func, set, signum)
                                     : delegate_real_sigaddset(file, line, func, set, signum);
}

int delegate_real_pthread_sigmask(const char *file, const int line, const char *func, int how, const sigset_t *set,
                                  sigset_t *oldset)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_sigmask(how, set, oldset);
}

int mock_pthread_sigmask(const char *file, const int line, const char *func, int how, const sigset_t *set,
                         sigset_t *oldset)
{
    return (_mock_signal != nullptr) ? _mock_signal->pthread_sigmask(file, line, func, how, set, oldset)
                                     : delegate_real_pthread_sigmask(file, line, func, how, set, oldset);
}

void (*delegate_real_signal(const char *file, const int line, const char *func, int signum, void (*handler)(int)))(int)
{
    (void)file;
    (void)line;
    (void)func;
    return signal(signum, handler);
}

void (*mock_signal(const char *file, const int line, const char *func, int signum, void (*handler)(int)))(int)
{
    return (_mock_signal != nullptr) ? _mock_signal->signal(file, line, func, signum, handler)
                                     : delegate_real_signal(file, line, func, signum, handler);
}

int delegate_real_raise(const char *file, const int line, const char *func, int signum)
{
    (void)file;
    (void)line;
    (void)func;
    return raise(signum);
}

int mock_raise(const char *file, const int line, const char *func, int signum)
{
    return (_mock_signal != nullptr) ? _mock_signal->raise(file, line, func, signum)
                                     : delegate_real_raise(file, line, func, signum);
}

#endif // _WIN32
