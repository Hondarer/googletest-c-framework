#include <mock_instance.h>
#include <mock_zlib.h>

using namespace testing;

Mock_zlib *_mock_zlib = nullptr;

Mock_zlib::Mock_zlib()
{
    ON_CALL(*this, deflateInit2_(_, _, _, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_deflateInit2_));
    ON_CALL(*this, inflateInit2_(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_inflateInit2_));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_zlib);
}

Mock_zlib::~Mock_zlib()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_zlib);
}
