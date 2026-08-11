#ifndef _MOCK_PTHREAD_H
#define _MOCK_PTHREAD_H

#ifndef _WIN32

    #include <pthread.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_pthread_mutex_init(const char *, const int, const char *, pthread_mutex_t *,
                                       const pthread_mutexattr_t *);
    extern int mock_pthread_mutex_destroy(const char *, const int, const char *, pthread_mutex_t *);
    extern int mock_pthread_mutex_lock(const char *, const int, const char *, pthread_mutex_t *);
    extern int mock_pthread_mutex_trylock(const char *, const int, const char *, pthread_mutex_t *);
    extern int mock_pthread_mutex_unlock(const char *, const int, const char *, pthread_mutex_t *);
    extern int mock_pthread_condattr_init(const char *, const int, const char *, pthread_condattr_t *);
    extern int mock_pthread_condattr_setclock(const char *, const int, const char *, pthread_condattr_t *, clockid_t);
    extern int mock_pthread_condattr_destroy(const char *, const int, const char *, pthread_condattr_t *);
    extern int mock_pthread_cond_init(const char *, const int, const char *, pthread_cond_t *,
                                      const pthread_condattr_t *);
    extern int mock_pthread_cond_destroy(const char *, const int, const char *, pthread_cond_t *);
    extern int mock_pthread_cond_wait(const char *, const int, const char *, pthread_cond_t *, pthread_mutex_t *);
    extern int mock_pthread_cond_timedwait(const char *, const int, const char *, pthread_cond_t *, pthread_mutex_t *,
                                           const struct timespec *);
    extern int mock_pthread_cond_signal(const char *, const int, const char *, pthread_cond_t *);
    extern int mock_pthread_cond_broadcast(const char *, const int, const char *, pthread_cond_t *);
    extern int mock_pthread_create(const char *, const int, const char *, pthread_t *, const pthread_attr_t *,
                                   void *(*)(void *), void *);
    extern int mock_pthread_join(const char *, const int, const char *, pthread_t, void **);
    extern int mock_pthread_tryjoin_np(const char *, const int, const char *, pthread_t, void **);
    extern int mock_pthread_detach(const char *, const int, const char *, pthread_t);
    extern int mock_pthread_getattr_np(const char *, const int, const char *, pthread_t, pthread_attr_t *);
    extern int mock_pthread_attr_getstack(const char *, const int, const char *, const pthread_attr_t *, void **,
                                          size_t *);
    extern int mock_pthread_attr_destroy(const char *, const int, const char *, pthread_attr_t *);
    extern pthread_t mock_pthread_self(const char *, const int, const char *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_PTHREAD_H

        #define pthread_mutex_init(mutex, attr) mock_pthread_mutex_init(__FILE__, __LINE__, __func__, mutex, attr)
        #define pthread_mutex_destroy(mutex)    mock_pthread_mutex_destroy(__FILE__, __LINE__, __func__, mutex)
        #define pthread_mutex_lock(mutex)       mock_pthread_mutex_lock(__FILE__, __LINE__, __func__, mutex)
        #define pthread_mutex_trylock(mutex)    mock_pthread_mutex_trylock(__FILE__, __LINE__, __func__, mutex)
        #define pthread_mutex_unlock(mutex)     mock_pthread_mutex_unlock(__FILE__, __LINE__, __func__, mutex)
        #define pthread_condattr_init(attr)     mock_pthread_condattr_init(__FILE__, __LINE__, __func__, attr)
        #define pthread_condattr_setclock(attr, clock) \
            mock_pthread_condattr_setclock(__FILE__, __LINE__, __func__, attr, clock)
        #define pthread_condattr_destroy(attr) mock_pthread_condattr_destroy(__FILE__, __LINE__, __func__, attr)
        #define pthread_cond_init(cond, attr)  mock_pthread_cond_init(__FILE__, __LINE__, __func__, cond, attr)
        #define pthread_cond_destroy(cond)     mock_pthread_cond_destroy(__FILE__, __LINE__, __func__, cond)
        #define pthread_cond_wait(cond, mutex) mock_pthread_cond_wait(__FILE__, __LINE__, __func__, cond, mutex)
        #define pthread_cond_timedwait(cond, mutex, abstime) \
            mock_pthread_cond_timedwait(__FILE__, __LINE__, __func__, cond, mutex, abstime)
        #define pthread_cond_signal(cond)    mock_pthread_cond_signal(__FILE__, __LINE__, __func__, cond)
        #define pthread_cond_broadcast(cond) mock_pthread_cond_broadcast(__FILE__, __LINE__, __func__, cond)
        #define pthread_create(thread, attr, start_routine, arg) \
            mock_pthread_create(__FILE__, __LINE__, __func__, thread, attr, start_routine, arg)
        #define pthread_join(thread, value_ptr) mock_pthread_join(__FILE__, __LINE__, __func__, thread, value_ptr)
        #define pthread_tryjoin_np(thread, value_ptr) \
            mock_pthread_tryjoin_np(__FILE__, __LINE__, __func__, thread, value_ptr)
        #define pthread_detach(thread)           mock_pthread_detach(__FILE__, __LINE__, __func__, thread)
        #define pthread_getattr_np(thread, attr) mock_pthread_getattr_np(__FILE__, __LINE__, __func__, thread, attr)
        #define pthread_attr_getstack(attr, stackaddr, stacksize) \
            mock_pthread_attr_getstack(__FILE__, __LINE__, __func__, attr, stackaddr, stacksize)
        #define pthread_attr_destroy(attr) mock_pthread_attr_destroy(__FILE__, __LINE__, __func__, attr)
        #define pthread_self()             mock_pthread_self(__FILE__, __LINE__, __func__)

    #else // _IN_OVERRIDE_HEADER_PTHREAD_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_pthread_mutex_init(const char *, const int, const char *, pthread_mutex_t *,
                                            const pthread_mutexattr_t *);
extern int delegate_real_pthread_mutex_destroy(const char *, const int, const char *, pthread_mutex_t *);
extern int delegate_real_pthread_mutex_lock(const char *, const int, const char *, pthread_mutex_t *);
extern int delegate_real_pthread_mutex_trylock(const char *, const int, const char *, pthread_mutex_t *);
extern int delegate_real_pthread_mutex_unlock(const char *, const int, const char *, pthread_mutex_t *);
extern int delegate_real_pthread_condattr_init(const char *, const int, const char *, pthread_condattr_t *);
extern int delegate_real_pthread_condattr_setclock(const char *, const int, const char *, pthread_condattr_t *,
                                                   clockid_t);
extern int delegate_real_pthread_condattr_destroy(const char *, const int, const char *, pthread_condattr_t *);
extern int delegate_real_pthread_cond_init(const char *, const int, const char *, pthread_cond_t *,
                                           const pthread_condattr_t *);
extern int delegate_real_pthread_cond_destroy(const char *, const int, const char *, pthread_cond_t *);
extern int delegate_real_pthread_cond_wait(const char *, const int, const char *, pthread_cond_t *, pthread_mutex_t *);
extern int delegate_real_pthread_cond_timedwait(const char *, const int, const char *, pthread_cond_t *,
                                                pthread_mutex_t *, const struct timespec *);
extern int delegate_real_pthread_cond_signal(const char *, const int, const char *, pthread_cond_t *);
extern int delegate_real_pthread_cond_broadcast(const char *, const int, const char *, pthread_cond_t *);
extern int delegate_real_pthread_create(const char *, const int, const char *, pthread_t *, const pthread_attr_t *,
                                        void *(*)(void *), void *);
extern int delegate_real_pthread_join(const char *, const int, const char *, pthread_t, void **);
extern int delegate_real_pthread_tryjoin_np(const char *, const int, const char *, pthread_t, void **);
extern int delegate_real_pthread_detach(const char *, const int, const char *, pthread_t);
extern int delegate_real_pthread_getattr_np(const char *, const int, const char *, pthread_t, pthread_attr_t *);
extern int delegate_real_pthread_attr_getstack(const char *, const int, const char *, const pthread_attr_t *, void **,
                                               size_t *);
extern int delegate_real_pthread_attr_destroy(const char *, const int, const char *, pthread_attr_t *);
extern pthread_t delegate_real_pthread_self(const char *, const int, const char *);

class Mock_pthread
{
  public:
    MOCK_METHOD(int, pthread_mutex_init,
                (const char *, const int, const char *, pthread_mutex_t *, const pthread_mutexattr_t *));
    MOCK_METHOD(int, pthread_mutex_destroy, (const char *, const int, const char *, pthread_mutex_t *));
    MOCK_METHOD(int, pthread_mutex_lock, (const char *, const int, const char *, pthread_mutex_t *));
    MOCK_METHOD(int, pthread_mutex_trylock, (const char *, const int, const char *, pthread_mutex_t *));
    MOCK_METHOD(int, pthread_mutex_unlock, (const char *, const int, const char *, pthread_mutex_t *));
    MOCK_METHOD(int, pthread_condattr_init, (const char *, const int, const char *, pthread_condattr_t *));
    MOCK_METHOD(int, pthread_condattr_setclock,
                (const char *, const int, const char *, pthread_condattr_t *, clockid_t));
    MOCK_METHOD(int, pthread_condattr_destroy, (const char *, const int, const char *, pthread_condattr_t *));
    MOCK_METHOD(int, pthread_cond_init,
                (const char *, const int, const char *, pthread_cond_t *, const pthread_condattr_t *));
    MOCK_METHOD(int, pthread_cond_destroy, (const char *, const int, const char *, pthread_cond_t *));
    MOCK_METHOD(int, pthread_cond_wait, (const char *, const int, const char *, pthread_cond_t *, pthread_mutex_t *));
    MOCK_METHOD(int, pthread_cond_timedwait,
                (const char *, const int, const char *, pthread_cond_t *, pthread_mutex_t *, const struct timespec *));
    MOCK_METHOD(int, pthread_cond_signal, (const char *, const int, const char *, pthread_cond_t *));
    MOCK_METHOD(int, pthread_cond_broadcast, (const char *, const int, const char *, pthread_cond_t *));
    MOCK_METHOD(int, pthread_create,
                (const char *, const int, const char *, pthread_t *, const pthread_attr_t *, void *(*)(void *),
                 void *));
    MOCK_METHOD(int, pthread_join, (const char *, const int, const char *, pthread_t, void **));
    MOCK_METHOD(int, pthread_tryjoin_np, (const char *, const int, const char *, pthread_t, void **));
    MOCK_METHOD(int, pthread_detach, (const char *, const int, const char *, pthread_t));
    MOCK_METHOD(int, pthread_getattr_np, (const char *, const int, const char *, pthread_t, pthread_attr_t *));
    MOCK_METHOD(int, pthread_attr_getstack,
                (const char *, const int, const char *, const pthread_attr_t *, void **, size_t *));
    MOCK_METHOD(int, pthread_attr_destroy, (const char *, const int, const char *, pthread_attr_t *));
    MOCK_METHOD(pthread_t, pthread_self, (const char *, const int, const char *));

    Mock_pthread();
    ~Mock_pthread();
};

extern Mock_pthread *_mock_pthread;

    #endif // _IN_OVERRIDE_HEADER_PTHREAD_H

#endif // _WIN32

#endif // _MOCK_PTHREAD_H
