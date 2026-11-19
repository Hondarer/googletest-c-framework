#include <mock_instance.h>
#ifdef _WIN32

    #include <mock_bcrypt.h>

using namespace testing;

Mock_bcrypt *_mock_bcrypt = nullptr;

Mock_bcrypt::Mock_bcrypt()
{
    ON_CALL(*this, BCryptDestroyKey(_, _, _, _)).WillByDefault(Invoke(delegate_real_BCryptDestroyKey));
    ON_CALL(*this, BCryptDestroyHash(_, _, _, _)).WillByDefault(Invoke(delegate_real_BCryptDestroyHash));
    ON_CALL(*this, BCryptCloseAlgorithmProvider(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_BCryptCloseAlgorithmProvider));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_bcrypt);
}

Mock_bcrypt::~Mock_bcrypt()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_bcrypt);
}

#endif // _WIN32
