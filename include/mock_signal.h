#ifndef _MOCK_SIGNAL_H
#define _MOCK_SIGNAL_H

#ifndef _WIN32

    #include <signal.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_sigaction(const char *, const int, const char *, int, const struct sigaction *, struct sigaction *);
    extern int mock_sigemptyset(const char *, const int, const char *, sigset_t *);
    extern void (*mock_signal(const char *, const int, const char *, int, void (*)(int)))(int);
    extern int mock_raise(const char *, const int, const char *, int);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_SIGNAL_H

        #define sigaction(signum, act, oldact) mock_sigaction(__FILE__, __LINE__, __func__, signum, act, oldact)
        #define sigemptyset(set)               mock_sigemptyset(__FILE__, __LINE__, __func__, set)
        #define signal(signum, handler)        mock_signal(__FILE__, __LINE__, __func__, signum, handler)
        #define raise(signum)                  mock_raise(__FILE__, __LINE__, __func__, signum)

    #else // _IN_OVERRIDE_HEADER_SIGNAL_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_sigaction(const char *, const int, const char *, int, const struct sigaction *,
                                   struct sigaction *);
extern int delegate_real_sigemptyset(const char *, const int, const char *, sigset_t *);
extern void (*delegate_real_signal(const char *, const int, const char *, int, void (*)(int)))(int);
extern int delegate_real_raise(const char *, const int, const char *, int);

class Mock_signal
{
  public:
    MOCK_METHOD(int, sigaction,
                (const char *, const int, const char *, int, const struct sigaction *, struct sigaction *));
    MOCK_METHOD(int, sigemptyset, (const char *, const int, const char *, sigset_t *));
    MOCK_METHOD(void (*)(int), signal, (const char *, const int, const char *, int, void (*)(int)));
    MOCK_METHOD(int, raise, (const char *, const int, const char *, int));

    Mock_signal();
    ~Mock_signal();
};

extern Mock_signal *_mock_signal;

    #endif // _IN_OVERRIDE_HEADER_SIGNAL_H

#endif // _WIN32

#endif // _MOCK_SIGNAL_H
