#include <test_com.h>
#include <mock_pthread.h>

#ifndef _WIN32

using namespace testing;

Mock_pthread *_mock_pthread = nullptr;

Mock_pthread::Mock_pthread()
{
    ON_CALL(*this, pthread_mutex_init(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_mutex_init));
    ON_CALL(*this, pthread_mutex_destroy(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_mutex_destroy));
    ON_CALL(*this, pthread_mutex_lock(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_mutex_lock));
    ON_CALL(*this, pthread_mutex_trylock(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_mutex_trylock));
    ON_CALL(*this, pthread_mutex_unlock(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_mutex_unlock));
    ON_CALL(*this, pthread_condattr_init(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_condattr_init));
    ON_CALL(*this, pthread_condattr_setclock(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_pthread_condattr_setclock));
    ON_CALL(*this, pthread_condattr_destroy(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_condattr_destroy));
    ON_CALL(*this, pthread_cond_init(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_cond_init));
    ON_CALL(*this, pthread_cond_destroy(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_cond_destroy));
    ON_CALL(*this, pthread_cond_wait(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_cond_wait));
    ON_CALL(*this, pthread_cond_timedwait(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_pthread_cond_timedwait));
    ON_CALL(*this, pthread_cond_signal(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_cond_signal));
    ON_CALL(*this, pthread_cond_broadcast(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_cond_broadcast));
    ON_CALL(*this, pthread_create(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_create));
    ON_CALL(*this, pthread_join(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_join));
    ON_CALL(*this, pthread_tryjoin_np(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_tryjoin_np));
    ON_CALL(*this, pthread_detach(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_detach));
    ON_CALL(*this, pthread_getattr_np(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_getattr_np));
    ON_CALL(*this, pthread_attr_getstack(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_attr_getstack));
    ON_CALL(*this, pthread_attr_destroy(_, _, _, _)).WillByDefault(Invoke(delegate_real_pthread_attr_destroy));
    ON_CALL(*this, pthread_self(_, _, _)).WillByDefault(Invoke(delegate_real_pthread_self));

    _mock_pthread = this;
}

Mock_pthread::~Mock_pthread()
{
    _mock_pthread = nullptr;
}

#endif // _WIN32
