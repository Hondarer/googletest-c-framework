#include <gmock/gmock.h>

#include <mock_stdlib.h>

using namespace testing;

Mock_stdlib *_mock_stdlib = nullptr;

Mock_stdlib::Mock_stdlib()
{
    ON_CALL(*this, calloc(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_calloc));

    _mock_stdlib = this;
}

Mock_stdlib::~Mock_stdlib()
{
    _mock_stdlib = nullptr;
}
