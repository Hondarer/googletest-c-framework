#include <mock_instance.h>
#include <test_com.h>
#include <sys/mock_eventfd.h>

#ifndef _WIN32

using namespace testing;

Mock_eventfd *_mock_eventfd = nullptr;

Mock_eventfd::Mock_eventfd()
{
    ON_CALL(*this, eventfd(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_eventfd));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_eventfd);
}

Mock_eventfd::~Mock_eventfd()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_eventfd);
}

int delegate_real_eventfd(const char *file, const int line, const char *func, unsigned int initial_value, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return eventfd(initial_value, flags);
}

int mock_eventfd(const char *file, const int line, const char *func, unsigned int initial_value, int flags)
{
    int mock_ret;

    if (_mock_eventfd != nullptr)
    {
        mock_ret = _mock_eventfd->eventfd(file, line, func, initial_value, flags);
    }
    else
    {
        mock_ret = delegate_real_eventfd(file, line, func, initial_value, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > eventfd");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
