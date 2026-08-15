#include <mock_instance.h>
#include <mock_stdlib.h>

using namespace testing;

Mock_stdlib *_mock_stdlib = nullptr;

Mock_stdlib::Mock_stdlib()
{
    ON_CALL(*this, malloc(_, _, _, _)).WillByDefault(Invoke(delegate_real_malloc));
    ON_CALL(*this, realloc(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_realloc));
    ON_CALL(*this, calloc(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_calloc));
    ON_CALL(*this, getenv(_, _, _, _)).WillByDefault(Invoke(delegate_real_getenv));
    ON_CALL(*this, atexit(_, _, _, _)).WillByDefault(Invoke(delegate_real_atexit));
#ifndef _WIN32
    ON_CALL(*this, realpath(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_realpath));
#endif // _WIN32
#ifndef _WIN32
    ON_CALL(*this, setenv(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_setenv));
    ON_CALL(*this, unsetenv(_, _, _, _)).WillByDefault(Invoke(delegate_real_unsetenv));
#endif // _WIN32

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_stdlib);
}

Mock_stdlib::~Mock_stdlib()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_stdlib);
}
