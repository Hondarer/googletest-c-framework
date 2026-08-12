#include <test_com.h>
#include <mock_dlfcn.h>

#ifndef _WIN32

using namespace testing;

Mock_dlfcn *_mock_dlfcn = nullptr;

Mock_dlfcn::Mock_dlfcn()
{
    ON_CALL(*this, dlopen(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_dlopen));
    ON_CALL(*this, dlsym(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_dlsym));
    ON_CALL(*this, dlclose(_, _, _, _)).WillByDefault(Invoke(delegate_real_dlclose));
    ON_CALL(*this, dladdr(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_dladdr));

    _mock_dlfcn = this;
}

Mock_dlfcn::~Mock_dlfcn()
{
    _mock_dlfcn = nullptr;
}

void *delegate_real_dlopen(const char *file, const int line, const char *func, const char *filename, int flags)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return dlopen(filename, flags);
}

void *mock_dlopen(const char *file, const int line, const char *func, const char *filename, int flags)
{
    void *result;

    if (_mock_dlfcn != nullptr)
    {
        result = _mock_dlfcn->dlopen(file, line, func, filename, flags);
    }
    else
    {
        result = delegate_real_dlopen(file, line, func, filename, flags);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > dlopen");
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
void *delegate_real_dlsym(const char *file, const int line, const char *func, void *handle, const char *symbol)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return dlsym(handle, symbol);
}

void *mock_dlsym(const char *file, const int line, const char *func, void *handle, const char *symbol)
{
    void *result;

    if (_mock_dlfcn != nullptr)
    {
        result = _mock_dlfcn->dlsym(file, line, func, handle, symbol);
    }
    else
    {
        result = delegate_real_dlsym(file, line, func, handle, symbol);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > dlsym");
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
int delegate_real_dlclose(const char *file, const int line, const char *func, void *handle)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return dlclose(handle);
}

int mock_dlclose(const char *file, const int line, const char *func, void *handle)
{
    int result;

    if (_mock_dlfcn != nullptr)
    {
        result = _mock_dlfcn->dlclose(file, line, func, handle);
    }
    else
    {
        result = delegate_real_dlclose(file, line, func, handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > dlclose");
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

int delegate_real_dladdr(const char *file, const int line, const char *func, const void *address, void *info)
{
    (void)file;
    (void)line;
    (void)func;

    return dladdr(address, static_cast<Dl_info *>(info));
}

int mock_dladdr(const char *file, const int line, const char *func, const void *address, void *info)
{
    int ret;

    if (_mock_dlfcn != nullptr)
    {
        ret = _mock_dlfcn->dladdr(file, line, func, address, info);
    }
    else
    {
        ret = delegate_real_dladdr(file, line, func, address, info);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > dladdr 0x%p, 0x%p", address, (void *)info);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, ret);
        }
        else
        {
            printf("\n");
        }
    }

    return ret;
}

#endif // _WIN32
