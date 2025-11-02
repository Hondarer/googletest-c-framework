#include <sys/mock_wait.h>

using namespace testing;

Mock_sys_wait *_mock_sys_wait = nullptr;

Mock_sys_wait::Mock_sys_wait()
{
    ON_CALL(*this, waitpid(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_waitpid));

        _mock_sys_wait = this;
}

Mock_sys_wait::~Mock_sys_wait()
{
    _mock_sys_wait = nullptr;
}
