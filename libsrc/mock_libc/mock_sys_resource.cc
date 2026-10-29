#include <mock_instance.h>
#include <test_com.h>
#include <sys/mock_resource.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_resource *_mock_sys_resource = nullptr;

Mock_sys_resource::Mock_sys_resource()
{
    ON_CALL(*this, setpriority(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_setpriority));
    ON_CALL(*this, getpriority(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_getpriority));
    ON_CALL(*this, setrlimit(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_setrlimit));
    ON_CALL(*this, getrlimit(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_getrlimit));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_sys_resource);
}

Mock_sys_resource::~Mock_sys_resource()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_sys_resource);
}

int delegate_real_setpriority(const char *file, const int line, const char *func, int which, id_t who, int prio)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return setpriority(static_cast<__priority_which_t>(which), who, prio);
}

int mock_setpriority(const char *file, const int line, const char *func, int which, id_t who, int prio)
{
    int mock_ret;

    if (_mock_sys_resource != nullptr)
    {
        mock_ret = _mock_sys_resource->setpriority(file, line, func, which, who, prio);
    }
    else
    {
        mock_ret = delegate_real_setpriority(file, line, func, which, who, prio);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > setpriority");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

int delegate_real_getpriority(const char *file, const int line, const char *func, int which, id_t who)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getpriority(static_cast<__priority_which_t>(which), who);
}

int mock_getpriority(const char *file, const int line, const char *func, int which, id_t who)
{
    int mock_ret;

    if (_mock_sys_resource != nullptr)
    {
        mock_ret = _mock_sys_resource->getpriority(file, line, func, which, who);
    }
    else
    {
        mock_ret = delegate_real_getpriority(file, line, func, which, who);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > getpriority");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

int delegate_real_setrlimit(const char *file, const int line, const char *func, int resource, const struct rlimit *rlim)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return setrlimit(static_cast<__rlimit_resource_t>(resource), rlim);
}

int mock_setrlimit(const char *file, const int line, const char *func, int resource, const struct rlimit *rlim)
{
    int mock_ret;

    if (_mock_sys_resource != nullptr)
    {
        mock_ret = _mock_sys_resource->setrlimit(file, line, func, resource, rlim);
    }
    else
    {
        mock_ret = delegate_real_setrlimit(file, line, func, resource, rlim);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > setrlimit");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

int delegate_real_getrlimit(const char *file, const int line, const char *func, int resource, struct rlimit *rlim)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return getrlimit(static_cast<__rlimit_resource_t>(resource), rlim);
}

int mock_getrlimit(const char *file, const int line, const char *func, int resource, struct rlimit *rlim)
{
    int mock_ret;

    if (_mock_sys_resource != nullptr)
    {
        mock_ret = _mock_sys_resource->getrlimit(file, line, func, resource, rlim);
    }
    else
    {
        mock_ret = delegate_real_getrlimit(file, line, func, resource, rlim);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > getrlimit");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
