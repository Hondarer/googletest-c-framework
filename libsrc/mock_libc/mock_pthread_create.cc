#include <test_com.h>
#include <mock_pthread.h>

#ifndef _WIN32

using namespace testing;

int delegate_real_pthread_create(const char *file, const int line, const char *func, pthread_t *thread,
                                 const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return pthread_create(thread, attr, start_routine, arg);
}

int mock_pthread_create(const char *file, const int line, const char *func, pthread_t *thread,
                        const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    int result;

    if (_mock_pthread != nullptr)
    {
        result = _mock_pthread->pthread_create(file, line, func, thread, attr, start_routine, arg);
    }
    else
    {
        result = delegate_real_pthread_create(file, line, func, thread, attr, start_routine, arg);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > pthread_create");
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
