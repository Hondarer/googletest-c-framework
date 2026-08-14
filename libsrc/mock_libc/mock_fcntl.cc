#include <test_com.h>
#include <mock_fcntl.h>

#ifndef _WIN32

using namespace testing;

Mock_fcntl *_mock_fcntl = nullptr;

Mock_fcntl::Mock_fcntl()
{
    ON_CALL(*this, open(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_open));
    ON_CALL(*this, fcntl(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_fcntl));
    _mock_fcntl = this;
}

Mock_fcntl::~Mock_fcntl()
{
    _mock_fcntl = nullptr;
}

/* 追加引数がポインターであるコマンドを判定する。
   fcntl は追加引数を int で読むコマンドとポインターで読むコマンドが混在するため、
   本物へ委譲するときに正しい型で渡し分ける必要がある。
   see: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html */
static bool takes_pointer_argument(int cmd)
{
    switch (cmd)
    {
    case F_GETLK:
    case F_SETLK:
    case F_SETLKW:
        return true;
    default:
        return false;
    }
}

int delegate_real_fcntl(const char *file, const int line, const char *func, int fd, int cmd, intptr_t arg)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    if (takes_pointer_argument(cmd))
    {
        return fcntl(fd, cmd, (void *)arg);
    }

    return fcntl(fd, cmd, (int)arg);
}

int mock_fcntl(const char *file, const int line, const char *func, int fd, int cmd, intptr_t arg)
{
    int mock_ret;

    if (_mock_fcntl != nullptr)
    {
        mock_ret = _mock_fcntl->fcntl(file, line, func, fd, cmd, arg);
    }
    else
    {
        mock_ret = delegate_real_fcntl(file, line, func, fd, cmd, arg);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fcntl");
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

#endif // _WIN32
