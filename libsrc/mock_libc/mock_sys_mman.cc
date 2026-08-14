#include <test_com.h>
#include <sys/mock_mman.h>

#ifndef _WIN32

using namespace testing;

Mock_sys_mman *_mock_sys_mman = nullptr;

Mock_sys_mman::Mock_sys_mman()
{
    ON_CALL(*this, mmap(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_mmap));
    ON_CALL(*this, munmap(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_munmap));
    ON_CALL(*this, msync(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_msync));
    ON_CALL(*this, mlock(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_mlock));
    ON_CALL(*this, munlock(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_munlock));
    ON_CALL(*this, mlockall(_, _, _, _)).WillByDefault(Invoke(delegate_real_mlockall));
    ON_CALL(*this, munlockall(_, _, _)).WillByDefault(Invoke(delegate_real_munlockall));

    _mock_sys_mman = this;
}

Mock_sys_mman::~Mock_sys_mman()
{
    _mock_sys_mman = nullptr;
}

void *delegate_real_mmap(const char *file, const int line, const char *func, void *addr, size_t length, int prot,
                         int flags, int fd, off_t offset)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return mmap(addr, length, prot, flags, fd, offset);
}

void *mock_mmap(const char *file, const int line, const char *func, void *addr, size_t length, int prot, int flags,
                int fd, off_t offset)
{
    void *mock_ret;

    if (_mock_sys_mman != nullptr)
    {
        mock_ret = _mock_sys_mman->mmap(file, line, func, addr, length, prot, flags, fd, offset);
    }
    else
    {
        mock_ret = delegate_real_mmap(file, line, func, addr, length, prot, flags, fd, offset);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > mmap");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
int delegate_real_munmap(const char *file, const int line, const char *func, void *addr, size_t length)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return munmap(addr, length);
}

int mock_munmap(const char *file, const int line, const char *func, void *addr, size_t length)
{
    int mock_ret;

    if (_mock_sys_mman != nullptr)
    {
        mock_ret = _mock_sys_mman->munmap(file, line, func, addr, length);
    }
    else
    {
        mock_ret = delegate_real_munmap(file, line, func, addr, length);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > munmap");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
int delegate_real_msync(const char *file, const int line, const char *func, void *addr, size_t length, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return msync(addr, length, flags);
}

int mock_msync(const char *file, const int line, const char *func, void *addr, size_t length, int flags)
{
    int mock_ret;

    if (_mock_sys_mman != nullptr)
    {
        mock_ret = _mock_sys_mman->msync(file, line, func, addr, length, flags);
    }
    else
    {
        mock_ret = delegate_real_msync(file, line, func, addr, length, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > msync");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

int delegate_real_mlock(const char *file, const int line, const char *func, const void *addr, size_t length)
{
    (void)file;
    (void)line;
    (void)func;
    return mlock(addr, length);
}

int mock_mlock(const char *file, const int line, const char *func, const void *addr, size_t length)
{
    if (_mock_sys_mman != nullptr)
    {
        return _mock_sys_mman->mlock(file, line, func, addr, length);
    }
    return delegate_real_mlock(file, line, func, addr, length);
}

int delegate_real_munlock(const char *file, const int line, const char *func, const void *addr, size_t length)
{
    (void)file;
    (void)line;
    (void)func;
    return munlock(addr, length);
}

int mock_munlock(const char *file, const int line, const char *func, const void *addr, size_t length)
{
    if (_mock_sys_mman != nullptr)
    {
        return _mock_sys_mman->munlock(file, line, func, addr, length);
    }
    return delegate_real_munlock(file, line, func, addr, length);
}

int delegate_real_mlockall(const char *file, const int line, const char *func, int flags)
{
    (void)file;
    (void)line;
    (void)func;
    return mlockall(flags);
}

int mock_mlockall(const char *file, const int line, const char *func, int flags)
{
    if (_mock_sys_mman != nullptr)
    {
        return _mock_sys_mman->mlockall(file, line, func, flags);
    }
    return delegate_real_mlockall(file, line, func, flags);
}

int delegate_real_munlockall(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;
    return munlockall();
}

int mock_munlockall(const char *file, const int line, const char *func)
{
    if (_mock_sys_mman != nullptr)
    {
        return _mock_sys_mman->munlockall(file, line, func);
    }
    return delegate_real_munlockall(file, line, func);
}

#endif // _WIN32
