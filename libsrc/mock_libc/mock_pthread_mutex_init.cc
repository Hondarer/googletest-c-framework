#include <test_com.h>
#include <mock_pthread.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_pthread_mutex_init(const char *file, const int line, const char *func, pthread_mutex_t *mutex,
                                     const pthread_mutexattr_t *attr)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return pthread_mutex_init(mutex, attr);
}

int mock_pthread_mutex_init(const char *file, const int line, const char *func, pthread_mutex_t *mutex,
                            const pthread_mutexattr_t *attr)
{
    int result;

    if (_mock_pthread != nullptr)
    {
        result = _mock_pthread->pthread_mutex_init(file, line, func, mutex, attr);
    }
    else
    {
        result = delegate_real_pthread_mutex_init(file, line, func, mutex, attr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > pthread_mutex_init");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

#endif // _WIN32
