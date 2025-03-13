#include <gmock/gmock.h>

#include <mock_string.h>

using namespace testing;

Mock_string *_mock_string = nullptr;

Mock_string::Mock_string()
{
    ON_CALL(*this, memset(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_memset));

    _mock_string = this;
}

Mock_string::~Mock_string()
{
    _mock_string = nullptr;
}
