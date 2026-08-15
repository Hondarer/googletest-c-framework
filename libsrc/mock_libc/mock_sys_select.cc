#include <mock_instance.h>
#include <test_com.h>
#include <sys/mock_select.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_select *_mock_sys_select = nullptr;

Mock_sys_select::Mock_sys_select()
{
    ON_CALL(*this, select(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_select));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_sys_select);
}

Mock_sys_select::~Mock_sys_select()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_sys_select);
}

#endif // _WIN32
