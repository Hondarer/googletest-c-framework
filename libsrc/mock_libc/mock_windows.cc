#ifdef _WIN32

#include <mock_windows.h>

using namespace testing;

Mock_windows *_mock_windows = nullptr;

Mock_windows::Mock_windows()
{
    switch_to_real_time();
    _mock_windows = this;
}

void Mock_windows::switch_to_real_time()
{
    ON_CALL(*this, GetTickCount64(_, _, _))
        .WillByDefault(Invoke(delegate_real_GetTickCount64));

    ON_CALL(*this, GetSystemTimeAsFileTime(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_GetSystemTimeAsFileTime));
}

void Mock_windows::switch_to_mock_time()
{
    ON_CALL(*this, GetTickCount64(_, _, _))
        .WillByDefault(Invoke(delegate_fake_GetTickCount64));

    ON_CALL(*this, GetSystemTimeAsFileTime(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_GetSystemTimeAsFileTime));
}

Mock_windows::~Mock_windows()
{
    _mock_windows = nullptr;
}

#endif // _WIN32
