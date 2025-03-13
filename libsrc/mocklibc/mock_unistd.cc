#include <gmock/gmock.h>

#include <mock_unistd.h>

using namespace testing;

Mock_unistd *_mock_unistd = nullptr;

Mock_unistd::Mock_unistd()
{
    ON_CALL(*this, access(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_access));

    _mock_unistd = this;
}

Mock_unistd::~Mock_unistd()
{
    _mock_unistd = nullptr;
}
