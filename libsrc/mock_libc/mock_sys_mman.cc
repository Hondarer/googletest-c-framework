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

    _mock_sys_mman = this;
}

Mock_sys_mman::~Mock_sys_mman()
{
    _mock_sys_mman = nullptr;
}

void *delegate_real_mmap(const char *file, const int line, const char *func, void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return mmap(addr, length, prot, flags, fd, offset);
}

void *mock_mmap(const char *file, const int line, const char *func, void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    void *result;

    if (_mock_sys_mman != nullptr)
    {
        result = _mock_sys_mman->mmap(file, line, func, addr, length, prot, flags, fd, offset);
    }
    else
    {
        result = delegate_real_mmap(file, line, func, addr, length, prot, flags, fd, offset);
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

    return result;
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
    int result;

    if (_mock_sys_mman != nullptr)
    {
        result = _mock_sys_mman->munmap(file, line, func, addr, length);
    }
    else
    {
        result = delegate_real_munmap(file, line, func, addr, length);
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

    return result;
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
    int result;

    if (_mock_sys_mman != nullptr)
    {
        result = _mock_sys_mman->msync(file, line, func, addr, length, flags);
    }
    else
    {
        result = delegate_real_msync(file, line, func, addr, length, flags);
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

    return result;
}

#endif // _WIN32
