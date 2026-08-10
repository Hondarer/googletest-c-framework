#include <mock_zlib.h>

using namespace testing;

Mock_zlib *_mock_zlib = nullptr;

Mock_zlib::Mock_zlib()
{
    ON_CALL(*this, deflateInit2_(_, _, _, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_deflateInit2_));
    ON_CALL(*this, inflateInit2_(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_inflateInit2_));

    _mock_zlib = this;
}

Mock_zlib::~Mock_zlib()
{
    _mock_zlib = nullptr;
}
