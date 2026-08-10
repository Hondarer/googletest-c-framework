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

    _mock_string = this;
}

Mock_string::~Mock_string()
{
    _mock_string = nullptr;
}
