#include <test_com.h>
#include <mock_ioctl.h>

#ifndef _WIN32

using namespace testing;

Mock_ioctl *_mock_ioctl = nullptr;

Mock_ioctl::Mock_ioctl()
{
    ON_CALL(*this, ioctl(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_ioctl));
    _mock_ioctl = this;
}

Mock_ioctl::~Mock_ioctl()
{
    _mock_ioctl = nullptr;
}

int delegate_real_ioctl(const char *file, const int line, const char *func, int fd, unsigned long request, void *arg)
{
    (void)file;
    (void)line;
    (void)func;
    return ioctl(fd, request, arg);
}

int mock_ioctl(const char *file, const int line, const char *func, int fd, unsigned long request, void *arg)
{
    return (_mock_ioctl != nullptr) ? _mock_ioctl->ioctl(file, line, func, fd, request, arg)
                                    : delegate_real_ioctl(file, line, func, fd, request, arg);
}

#endif // _WIN32
