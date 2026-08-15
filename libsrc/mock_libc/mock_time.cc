#include <mock_instance.h>
#include <mock_time.h>

using namespace testing;

Mock_time *_mock_time = nullptr;

Mock_time::Mock_time()
{
    switch_to_real_time();
    TESTFW_REGISTER_MOCK_INSTANCE(_mock_time);
}

#ifndef _WIN32

void Mock_time::switch_to_real_time()
{
    ON_CALL(*this, clock_gettime(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_clock_gettime));

    ON_CALL(*this, gmtime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_gmtime_r));

    ON_CALL(*this, localtime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_localtime_r));

    ON_CALL(*this, ctime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_ctime_r));

    ON_CALL(*this, nanosleep(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_nanosleep));
}

void Mock_time::switch_to_mock_time()
{
    ON_CALL(*this, clock_gettime(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_clock_gettime));

    ON_CALL(*this, gmtime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_gmtime_r));

    ON_CALL(*this, localtime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_localtime_r));

    ON_CALL(*this, ctime_r(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_ctime_r));

    ON_CALL(*this, nanosleep(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_nanosleep));
}

#else

void Mock_time::switch_to_real_time()
{
    ON_CALL(*this, gmtime_s(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_gmtime_s));

    ON_CALL(*this, localtime_s(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_localtime_s));

    ON_CALL(*this, ctime_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_ctime_s));
}

void Mock_time::switch_to_mock_time()
{
    ON_CALL(*this, gmtime_s(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_gmtime_s));

    ON_CALL(*this, localtime_s(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_localtime_s));

    ON_CALL(*this, ctime_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_ctime_s));
}

#endif

Mock_time::~Mock_time()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_time);
}
