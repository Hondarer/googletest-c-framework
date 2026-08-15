#include <mock_instance.h>
#ifndef _WIN32

    #include <sys/mock_wait.h>

using namespace testing;

Mock_sys_wait *_mock_sys_wait = nullptr;

Mock_sys_wait::Mock_sys_wait()
{
    ON_CALL(*this, waitpid(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_waitpid));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_sys_wait);
}

Mock_sys_wait::~Mock_sys_wait()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_sys_wait);
}

#endif // _WIN32
