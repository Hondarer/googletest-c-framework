#include <test_com.h>
#include <mock_fcntl.h>

#ifndef _WIN32

using namespace testing;

Mock_fcntl *_mock_fcntl = nullptr;

Mock_fcntl::Mock_fcntl()
{
    ON_CALL(*this, open(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_open));
    _mock_fcntl = this;
}

Mock_fcntl::~Mock_fcntl()
{
    _mock_fcntl = nullptr;
}

#endif // _WIN32
