#include <mock_time.h>

using namespace testing;

Mock_time *_mock_time = nullptr;

Mock_time::Mock_time()
{
    switch_to_real_time();
    _mock_time = this;
}

#ifndef _WIN32

void Mock_time::switch_to_real_time()
{
    ON_CALL(*this, clock_gettime(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_clock_gettime));

    ON_CALL(*this, gmtime_r(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_gmtime_r));

    ON_CALL(*this, ctime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_ctime_r));
}

void Mock_time::switch_to_mock_time()
{
    ON_CALL(*this, clock_gettime(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_clock_gettime));

    ON_CALL(*this, gmtime_r(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_gmtime_r));

    ON_CALL(*this, ctime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_ctime_r));
}

#else

void Mock_time::switch_to_real_time()
{
    ON_CALL(*this, gmtime_s(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_gmtime_s));

    ON_CALL(*this, ctime_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_ctime_s));
}

void Mock_time::switch_to_mock_time()
{
    ON_CALL(*this, gmtime_s(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_gmtime_s));

    ON_CALL(*this, ctime_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_ctime_s));
}

#endif

Mock_time::~Mock_time()
{
    _mock_time = nullptr;
}
