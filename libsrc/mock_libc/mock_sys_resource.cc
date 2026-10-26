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

#endif // _WIN32
