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
    extern int mock_pthread_create(const char *, const int, const char *, pthread_t *, const pthread_attr_t *,
                                   void *(*)(void *), void *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_PTHREAD_H

        #define pthread_mutex_init(mutex, attr) mock_pthread_mutex_init(__FILE__, __LINE__, __func__, mutex, attr)
        #define pthread_create(thread, attr, start_routine, arg) \
            mock_pthread_create(__FILE__, __LINE__, __func__, thread, attr, start_routine, arg)

    #else // _IN_OVERRIDE_HEADER_PTHREAD_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_pthread_mutex_init(const char *, const int, const char *, pthread_mutex_t *,
                                            const pthread_mutexattr_t *);
extern int delegate_real_pthread_create(const char *, const int, const char *, pthread_t *, const pthread_attr_t *,
                                        void *(*)(void *), void *);

class Mock_pthread
{
  public:
    MOCK_METHOD(int, pthread_mutex_init,
                (const char *, const int, const char *, pthread_mutex_t *, const pthread_mutexattr_t *));
    MOCK_METHOD(int, pthread_create,
                (const char *, const int, const char *, pthread_t *, const pthread_attr_t *, void *(*)(void *),
                 void *));

    Mock_pthread();
    ~Mock_pthread();
};

extern Mock_pthread *_mock_pthread;

    #endif // _IN_OVERRIDE_HEADER_PTHREAD_H

#endif // _WIN32

#endif // _MOCK_PTHREAD_H
