#include <mock_stdlib.h>

using namespace testing;

Mock_stdlib *_mock_stdlib = nullptr;

Mock_stdlib::Mock_stdlib()
{
    ON_CALL(*this, calloc(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_calloc));
    ON_CALL(*this, getenv(_, _, _, _)).WillByDefault(Invoke(delegate_real_getenv));
#ifndef _WIN32
    ON_CALL(*this, setenv(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_setenv));
    ON_CALL(*this, unsetenv(_, _, _, _)).WillByDefault(Invoke(delegate_real_unsetenv));
#endif // _WIN32

    _mock_stdlib = this;
}

Mock_stdlib::~Mock_stdlib()
{
    _mock_stdlib = nullptr;
}
