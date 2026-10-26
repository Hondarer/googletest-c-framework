#include <mock_instance.h>
#include <test_com.h>
#include <mock_sched.h>

#ifndef _WIN32

using namespace testing;

Mock_sched *_mock_sched = nullptr;

Mock_sched::Mock_sched()
{
    ON_CALL(*this, sched_setscheduler(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sched_setscheduler));
    ON_CALL(*this, sched_getscheduler(_, _, _, _)).WillByDefault(Invoke(delegate_real_sched_getscheduler));
    ON_CALL(*this, sched_getparam(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_sched_getparam));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_sched);
}

Mock_sched::~Mock_sched()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_sched);
}

int delegate_real_sched_setscheduler(const char *file, const int line, const char *func, pid_t pid, int policy, const struct sched_param *param)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return sched_setscheduler(pid, policy, param);
}

int mock_sched_setscheduler(const char *file, const int line, const char *func, pid_t pid, int policy, const struct sched_param *param)
{
    int mock_ret;

    if (_mock_sched != nullptr)
    {
        mock_ret = _mock_sched->sched_setscheduler(file, line, func, pid, policy, param);
    }
    else
    {
        mock_ret = delegate_real_sched_setscheduler(file, line, func, pid, policy, param);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sched_setscheduler");
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

int delegate_real_sched_getscheduler(const char *file, const int line, const char *func, pid_t pid)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return sched_getscheduler(pid);
}

int mock_sched_getscheduler(const char *file, const int line, const char *func, pid_t pid)
{
    int mock_ret;

    if (_mock_sched != nullptr)
    {
        mock_ret = _mock_sched->sched_getscheduler(file, line, func, pid);
    }
    else
    {
        mock_ret = delegate_real_sched_getscheduler(file, line, func, pid);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sched_getscheduler");
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

int delegate_real_sched_getparam(const char *file, const int line, const char *func, pid_t pid, struct sched_param *param)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return sched_getparam(pid, param);
}

int mock_sched_getparam(const char *file, const int line, const char *func, pid_t pid, struct sched_param *param)
{
    int mock_ret;

    if (_mock_sched != nullptr)
    {
        mock_ret = _mock_sched->sched_getparam(file, line, func, pid, param);
    }
    else
    {
        mock_ret = delegate_real_sched_getparam(file, line, func, pid, param);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sched_getparam");
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
