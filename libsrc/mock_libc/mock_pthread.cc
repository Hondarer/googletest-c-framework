#include <test_com.h>
#include <mock_pthread.h>

#ifndef _WIN32

using namespace testing;

Mock_pthread *_mock_pthread = nullptr;

Mock_pthread::Mock_pthread()
{
    ON_CALL(*this, pthread_mutex_init(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_mutex_init));
    ON_CALL(*this, pthread_create(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_create));

    _mock_pthread = this;
}

Mock_pthread::~Mock_pthread()
{
    _mock_pthread = nullptr;
}

#endif // _WIN32
