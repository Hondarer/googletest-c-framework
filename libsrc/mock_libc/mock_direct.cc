#include <mock_instance.h>
#ifdef _WIN32

    #include <mock_direct.h>

using namespace testing;

Mock_direct *_mock_direct = nullptr;

Mock_direct::Mock_direct()
{
    ON_CALL(*this, wmkdir(_, _, _, _)).WillByDefault(Invoke(delegate_real_wmkdir));
    ON_CALL(*this, wrmdir(_, _, _, _)).WillByDefault(Invoke(delegate_real_wrmdir));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_direct);
}

Mock_direct::~Mock_direct()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_direct);
}

#endif // _WIN32
