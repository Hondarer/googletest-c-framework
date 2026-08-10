#include <test_com.h>
#include <sys/mock_file.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_file *_mock_sys_file = nullptr;

Mock_sys_file::Mock_sys_file()
{
    ON_CALL(*this, flock(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_flock));

    _mock_sys_file = this;
}

Mock_sys_file::~Mock_sys_file()
{
    _mock_sys_file = nullptr;
}

#endif // _WIN32
