#include <mock_instance.h>
#include <sys/mock_stat.h>

using namespace testing;

Mock_sys_stat *_mock_sys_stat = nullptr;

Mock_sys_stat::Mock_sys_stat()
{
    ON_CALL(*this, stat(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_stat));
#ifndef _WIN32
    ON_CALL(*this, fstat(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_fstat));
    ON_CALL(*this, mkdir(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_mkdir));
#endif // _WIN32

#ifdef _WIN32
    ON_CALL(*this, stat64(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_stat64));
#endif

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_sys_stat);
}

Mock_sys_stat::~Mock_sys_stat()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_sys_stat);
}
