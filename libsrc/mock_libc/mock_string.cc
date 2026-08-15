#include <mock_instance.h>
#include <mock_string.h>

using namespace testing;

Mock_string *_mock_string = nullptr;

Mock_string::Mock_string()
{
    ON_CALL(*this, memset(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_memset));
#ifndef _WIN32
    ON_CALL(*this, strdup(_, _, _, _)).WillByDefault(Invoke(delegate_real_strdup));
    ON_CALL(*this, strerror_r(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_strerror_r));
#endif // _WIN32

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_string);
}

Mock_string::~Mock_string()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_string);
}
