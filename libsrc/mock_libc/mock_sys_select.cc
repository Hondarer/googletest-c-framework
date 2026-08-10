#include <test_com.h>
#include <sys/mock_select.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_select *_mock_sys_select = nullptr;

Mock_sys_select::Mock_sys_select()
{
    ON_CALL(*this, select(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_select));

    _mock_sys_select = this;
}

Mock_sys_select::~Mock_sys_select()
{
    _mock_sys_select = nullptr;
}

#endif // _WIN32
