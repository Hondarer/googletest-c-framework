#include <mock_instance.h>
#ifdef _WIN32

    #include <mock_shellapi.h>

using namespace testing;

Mock_shellapi *_mock_shellapi = nullptr;

Mock_shellapi::Mock_shellapi()
{
    ON_CALL(*this, ShellExecuteExW(_, _, _, _)).WillByDefault(Invoke(delegate_real_ShellExecuteExW));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_shellapi);
}

Mock_shellapi::~Mock_shellapi()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_shellapi);
}

#endif // _WIN32
