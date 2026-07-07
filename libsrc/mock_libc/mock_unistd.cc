#include <mock_unistd.h>

using namespace testing;

Mock_unistd *_mock_unistd = nullptr;

#ifndef _WIN32

Mock_unistd::Mock_unistd()
{
    ON_CALL(*this, access(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_access));
    ON_CALL(*this, fork(_, _, _))
        .WillByDefault(Invoke(delegate_real_fork));
    ON_CALL(*this, mkstemp(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_mkstemp));
    ON_CALL(*this, lseek(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_lseek));
    ON_CALL(*this, close(_, _, _, _)).WillByDefault(Invoke(delegate_real_close));
    ON_CALL(*this, dup(_, _, _, _)).WillByDefault(Invoke(delegate_real_dup));
    ON_CALL(*this, dup2(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_dup2));
    ON_CALL(*this, read(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_read));
    ON_CALL(*this, write(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_write));

    _mock_unistd = this;
}

#else // _WIN32

Mock_unistd::Mock_unistd()
{
    ON_CALL(*this, _lseeki64(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real__lseeki64));
    ON_CALL(*this, _close(_, _, _, _)).WillByDefault(Invoke(delegate_real__close));
    ON_CALL(*this, _dup(_, _, _, _)).WillByDefault(Invoke(delegate_real__dup));
    ON_CALL(*this, _dup2(_, _, _, _, _)).WillByDefault(Invoke(delegate_real__dup2));
    ON_CALL(*this, _read(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real__read));
    ON_CALL(*this, _write(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real__write));

    _mock_unistd = this;
}

#endif // _WIN32

Mock_unistd::~Mock_unistd()
{
    _mock_unistd = nullptr;
}
