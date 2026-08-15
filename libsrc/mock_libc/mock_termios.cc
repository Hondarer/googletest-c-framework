#include <mock_instance.h>
#include <test_com.h>
#include <mock_termios.h>

#ifndef _WIN32

using namespace testing;

Mock_termios *_mock_termios = nullptr;

Mock_termios::Mock_termios()
{
    ON_CALL(*this, tcgetattr(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_tcgetattr));
    ON_CALL(*this, tcsetattr(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_tcsetattr));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_termios);
}

Mock_termios::~Mock_termios()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_termios);
}

#endif // _WIN32
