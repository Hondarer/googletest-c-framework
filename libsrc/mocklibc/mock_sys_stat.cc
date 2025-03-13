#include <gmock/gmock.h>

#include <sys/mock_stat.h>

using namespace testing;

Mock_sys_stat *_mock_sys_stat = nullptr;

Mock_sys_stat::Mock_sys_stat()
{
    ON_CALL(*this, stat(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_stat));
    _mock_sys_stat = this;
}

Mock_sys_stat::~Mock_sys_stat()
{
    _mock_sys_stat = nullptr;
}
