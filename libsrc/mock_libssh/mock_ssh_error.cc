#include <test_com.h>
#include <mock_libssh.h>

using namespace testing;

/* フェイク エラー メッセージ用の静的バッファー */
static const char *fake_error_message = "No error";

/* ========================================
 * ssh_get_error
 * ======================================== */

const char *delegate_fake_ssh_get_error(const char *file, const int line, const char *func, void *error)
{
    (void)file;
    (void)line;
    (void)func;
    (void)error;

    return fake_error_message;
}

const char *delegate_real_ssh_get_error(const char *file, const int line, const char *func, void *error)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_get_error(error);
#else
    return delegate_fake_ssh_get_error(file, line, func, error);
#endif
}

const char *mock_ssh_get_error(const char *file, const int line, const char *func, void *error)
{
    const char *mock_ret;

    if (_mock_libssh != nullptr)
    {
        mock_ret = _mock_libssh->ssh_get_error(file, line, func, error);
    }
    else
    {
        mock_ret = delegate_real_ssh_get_error(file, line, func, error);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_get_error %p", error);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %s\n", file, line, mock_ret ? mock_ret : "(null)");
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

/* ========================================
 * ssh_get_error_code
 * ======================================== */

int delegate_fake_ssh_get_error_code(const char *file, const int line, const char *func, void *error)
{
    (void)file;
    (void)line;
    (void)func;
    (void)error;

    return SSH_OK;
}

int delegate_real_ssh_get_error_code(const char *file, const int line, const char *func, void *error)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_get_error_code(error);
#else
    return delegate_fake_ssh_get_error_code(file, line, func, error);
#endif
}

int mock_ssh_get_error_code(const char *file, const int line, const char *func, void *error)
{
    int mock_ret;

    if (_mock_libssh != nullptr)
    {
        mock_ret = _mock_libssh->ssh_get_error_code(file, line, func, error);
    }
    else
    {
        mock_ret = delegate_real_ssh_get_error_code(file, line, func, error);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_get_error_code %p", error);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
