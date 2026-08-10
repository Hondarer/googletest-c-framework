#include <test_com.h>
#include <mock_termios.h>

#ifndef _WIN32

using namespace testing;

Mock_termios *_mock_termios = nullptr;

Mock_termios::Mock_termios()
{
    ON_CALL(*this, tcgetattr(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_tcgetattr));
    ON_CALL(*this, tcsetattr(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_tcsetattr));

    _mock_termios = this;
}

Mock_termios::~Mock_termios()
{
    _mock_termios = nullptr;
}

#endif // _WIN32
