#include <test_com.h>
#include <mock_pthread.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_pthread_mutex_destroy(const char *file, const int line, const char *func, pthread_mutex_t *mutex)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_mutex_destroy(mutex);
}

int mock_pthread_mutex_destroy(const char *file, const int line, const char *func, pthread_mutex_t *mutex)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_mutex_destroy(file, line, func, mutex)
                                      : delegate_real_pthread_mutex_destroy(file, line, func, mutex);
}

    #define DEFINE_PTHREAD_MUTEX_OP(name) \
        int delegate_real_##name(const char *file, const int line, const char *func, pthread_mutex_t *mutex) \
        { \
            (void)file; \
            (void)line; \
            (void)func; \
            return name(mutex); \
        } \
        int mock_##name(const char *file, const int line, const char *func, pthread_mutex_t *mutex) \
        { \
            return (_mock_pthread != nullptr) ? _mock_pthread->name(file, line, func, mutex) \
                                              : delegate_real_##name(file, line, func, mutex); \
        }

DEFINE_PTHREAD_MUTEX_OP(pthread_mutex_lock)
DEFINE_PTHREAD_MUTEX_OP(pthread_mutex_trylock)
DEFINE_PTHREAD_MUTEX_OP(pthread_mutex_unlock)

int delegate_real_pthread_condattr_init(const char *file, const int line, const char *func, pthread_condattr_t *attr)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_condattr_init(attr);
}

int mock_pthread_condattr_init(const char *file, const int line, const char *func, pthread_condattr_t *attr)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_condattr_init(file, line, func, attr)
                                      : delegate_real_pthread_condattr_init(file, line, func, attr);
}

int delegate_real_pthread_condattr_setclock(const char *file, const int line, const char *func,
                                            pthread_condattr_t *attr, clockid_t clock_id)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_condattr_setclock(attr, clock_id);
}

int mock_pthread_condattr_setclock(const char *file, const int line, const char *func, pthread_condattr_t *attr,
                                   clockid_t clock_id)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_condattr_setclock(file, line, func, attr, clock_id)
                                      : delegate_real_pthread_condattr_setclock(file, line, func, attr, clock_id);
}

int delegate_real_pthread_condattr_destroy(const char *file, const int line, const char *func, pthread_condattr_t *attr)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_condattr_destroy(attr);
}

int mock_pthread_condattr_destroy(const char *file, const int line, const char *func, pthread_condattr_t *attr)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_condattr_destroy(file, line, func, attr)
                                      : delegate_real_pthread_condattr_destroy(file, line, func, attr);
}

    #define DEFINE_PTHREAD_COND_OP(name, call) \
        int delegate_real_##name(const char *file, const int line, const char *func, pthread_cond_t *cond) \
        { \
            (void)file; \
            (void)line; \
            (void)func; \
            return call; \
        } \
        int mock_##name(const char *file, const int line, const char *func, pthread_cond_t *cond) \
        { \
            return (_mock_pthread != nullptr) ? _mock_pthread->name(file, line, func, cond) \
                                              : delegate_real_##name(file, line, func, cond); \
        }

DEFINE_PTHREAD_COND_OP(pthread_cond_destroy, pthread_cond_destroy(cond))
DEFINE_PTHREAD_COND_OP(pthread_cond_signal, pthread_cond_signal(cond))
DEFINE_PTHREAD_COND_OP(pthread_cond_broadcast, pthread_cond_broadcast(cond))

int delegate_real_pthread_cond_init(const char *file, const int line, const char *func, pthread_cond_t *cond,
                                    const pthread_condattr_t *attr)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_cond_init(cond, attr);
}

int mock_pthread_cond_init(const char *file, const int line, const char *func, pthread_cond_t *cond,
                           const pthread_condattr_t *attr)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_cond_init(file, line, func, cond, attr)
                                      : delegate_real_pthread_cond_init(file, line, func, cond, attr);
}

int delegate_real_pthread_cond_wait(const char *file, const int line, const char *func, pthread_cond_t *cond,
                                    pthread_mutex_t *mutex)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_cond_wait(cond, mutex);
}

int mock_pthread_cond_wait(const char *file, const int line, const char *func, pthread_cond_t *cond,
                           pthread_mutex_t *mutex)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_cond_wait(file, line, func, cond, mutex)
                                      : delegate_real_pthread_cond_wait(file, line, func, cond, mutex);
}

int delegate_real_pthread_cond_timedwait(const char *file, const int line, const char *func, pthread_cond_t *cond,
                                         pthread_mutex_t *mutex, const struct timespec *abstime)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_cond_timedwait(cond, mutex, abstime);
}

int mock_pthread_cond_timedwait(const char *file, const int line, const char *func, pthread_cond_t *cond,
                                pthread_mutex_t *mutex, const struct timespec *abstime)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_cond_timedwait(file, line, func, cond, mutex, abstime)
                                      : delegate_real_pthread_cond_timedwait(file, line, func, cond, mutex, abstime);
}

    #define DEFINE_PTHREAD_THREAD_OP(name, call) \
        int delegate_real_##name(const char *file, const int line, const char *func, pthread_t thread, void **value) \
        { \
            (void)file; \
            (void)line; \
            (void)func; \
            return call; \
        } \
        int mock_##name(const char *file, const int line, const char *func, pthread_t thread, void **value) \
        { \
            return (_mock_pthread != nullptr) ? _mock_pthread->name(file, line, func, thread, value) \
                                              : delegate_real_##name(file, line, func, thread, value); \
        }

DEFINE_PTHREAD_THREAD_OP(pthread_join, pthread_join(thread, value))
DEFINE_PTHREAD_THREAD_OP(pthread_tryjoin_np, pthread_tryjoin_np(thread, value))

int delegate_real_pthread_detach(const char *file, const int line, const char *func, pthread_t thread)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_detach(thread);
}

int mock_pthread_detach(const char *file, const int line, const char *func, pthread_t thread)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_detach(file, line, func, thread)
                                      : delegate_real_pthread_detach(file, line, func, thread);
}

int delegate_real_pthread_getattr_np(const char *file, const int line, const char *func, pthread_t thread,
                                     pthread_attr_t *attr)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_getattr_np(thread, attr);
}

int mock_pthread_getattr_np(const char *file, const int line, const char *func, pthread_t thread, pthread_attr_t *attr)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_getattr_np(file, line, func, thread, attr)
                                      : delegate_real_pthread_getattr_np(file, line, func, thread, attr);
}

int delegate_real_pthread_attr_getstack(const char *file, const int line, const char *func, const pthread_attr_t *attr,
                                        void **stackaddr, size_t *stacksize)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_attr_getstack(attr, stackaddr, stacksize);
}

int mock_pthread_attr_getstack(const char *file, const int line, const char *func, const pthread_attr_t *attr,
                               void **stackaddr, size_t *stacksize)
{
    return (_mock_pthread != nullptr)
               ? _mock_pthread->pthread_attr_getstack(file, line, func, attr, stackaddr, stacksize)
               : delegate_real_pthread_attr_getstack(file, line, func, attr, stackaddr, stacksize);
}

int delegate_real_pthread_attr_destroy(const char *file, const int line, const char *func, pthread_attr_t *attr)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_attr_destroy(attr);
}

int mock_pthread_attr_destroy(const char *file, const int line, const char *func, pthread_attr_t *attr)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_attr_destroy(file, line, func, attr)
                                      : delegate_real_pthread_attr_destroy(file, line, func, attr);
}

pthread_t delegate_real_pthread_self(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;
    return pthread_self();
}

pthread_t mock_pthread_self(const char *file, const int line, const char *func)
{
    return (_mock_pthread != nullptr) ? _mock_pthread->pthread_self(file, line, func)
                                      : delegate_real_pthread_self(file, line, func);
}

    #undef DEFINE_PTHREAD_MUTEX_OP
    #undef DEFINE_PTHREAD_COND_OP
    #undef DEFINE_PTHREAD_THREAD_OP

#endif // _WIN32
