#include <mock_instance.h>
#include <mock_unistd.h>

using namespace testing;

Mock_unistd *_mock_unistd = nullptr;

#ifndef _WIN32

Mock_unistd::Mock_unistd()
{
    ON_CALL(*this, access(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_access));
    ON_CALL(*this, getcwd(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_getcwd));
    ON_CALL(*this, ftruncate(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_ftruncate));
    ON_CALL(*this, fsync(_, _, _, _)).WillByDefault(Invoke(delegate_real_fsync));
    ON_CALL(*this, fork(_, _, _)).WillByDefault(Invoke(delegate_real_fork));
    ON_CALL(*this, mkstemp(_, _, _, _)).WillByDefault(Invoke(delegate_real_mkstemp));
    ON_CALL(*this, mkostemp(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_mkostemp));
    ON_CALL(*this, unlink(_, _, _, _)).WillByDefault(Invoke(delegate_real_unlink));
    ON_CALL(*this, lseek(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_lseek));
    ON_CALL(*this, close(_, _, _, _)).WillByDefault(Invoke(delegate_real_close));
    ON_CALL(*this, dup(_, _, _, _)).WillByDefault(Invoke(delegate_real_dup));
    ON_CALL(*this, dup2(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_dup2));
    ON_CALL(*this, read(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_read));
    ON_CALL(*this, write(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_write));
    ON_CALL(*this, geteuid(_, _, _)).WillByDefault(Invoke(delegate_real_geteuid));
    ON_CALL(*this, getpid(_, _, _)).WillByDefault(Invoke(delegate_real_getpid));
    ON_CALL(*this, kill(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_kill));
    ON_CALL(*this, pipe(_, _, _, _)).WillByDefault(Invoke(delegate_real_pipe));
    ON_CALL(*this, execve(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_execve));
    ON_CALL(*this, readlink(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_readlink));
    ON_CALL(*this, usleep(_, _, _, _)).WillByDefault(Invoke(delegate_real_usleep));
    ON_CALL(*this, sched_yield(_, _, _)).WillByDefault(Invoke(delegate_real_sched_yield));
    ON_CALL(*this, rmdir(_, _, _, _)).WillByDefault(Invoke(delegate_real_rmdir));
    ON_CALL(*this, isatty(_, _, _, _)).WillByDefault(Invoke(delegate_real_isatty));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_unistd);
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

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_unistd);
}

#endif // _WIN32

Mock_unistd::~Mock_unistd()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_unistd);
}
