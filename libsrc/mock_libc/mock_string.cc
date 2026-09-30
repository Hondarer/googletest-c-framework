#include <mock_instance.h>
#include <mock_string.h>

using namespace testing;

Mock_string *_mock_string = nullptr;

Mock_string::Mock_string()
{
    ON_CALL(*this, memcpy(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_memcpy));
    ON_CALL(*this, memmove(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_memmove));
    ON_CALL(*this, memset(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_memset));
    ON_CALL(*this, strchr(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_strchr));
    ON_CALL(*this, strcmp(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_strcmp));
    ON_CALL(*this, strlen(_, _, _, _)).WillByDefault(Invoke(delegate_real_strlen));
    ON_CALL(*this, strncmp(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_strncmp));
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
