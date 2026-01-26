#include <test_com.h>
#include <mock_libssh.h>

using namespace testing;

/* ========================================
 * ssh_userauth_password
 * ======================================== */

int delegate_fake_ssh_userauth_password(const char *file, const int line, const char *func,
                                         ssh_session session, const char *username, const char *password)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;
    (void)username;
    (void)password;

    /* 認証成功を返す */
    return SSH_AUTH_SUCCESS;
}

int delegate_real_ssh_userauth_password(const char *file, const int line, const char *func,
                                         ssh_session session, const char *username, const char *password)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_userauth_password(session, username, password);
#else
    return delegate_fake_ssh_userauth_password(file, line, func, session, username, password);
#endif
}

int mock_ssh_userauth_password(const char *file, const int line, const char *func,
                                ssh_session session, const char *username, const char *password)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_userauth_password(file, line, func, session, username, password);
    }
    else
    {
        result = delegate_real_ssh_userauth_password(file, line, func, session, username, password);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_userauth_password %p, user=%s", (void *)session, username ? username : "(null)");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * ssh_userauth_publickey_auto
 * ======================================== */

int delegate_fake_ssh_userauth_publickey_auto(const char *file, const int line, const char *func,
                                               ssh_session session, const char *username, const char *passphrase)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;
    (void)username;
    (void)passphrase;

    /* 認証成功を返す */
    return SSH_AUTH_SUCCESS;
}

int delegate_real_ssh_userauth_publickey_auto(const char *file, const int line, const char *func,
                                               ssh_session session, const char *username, const char *passphrase)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_userauth_publickey_auto(session, username, passphrase);
#else
    return delegate_fake_ssh_userauth_publickey_auto(file, line, func, session, username, passphrase);
#endif
}

int mock_ssh_userauth_publickey_auto(const char *file, const int line, const char *func,
                                      ssh_session session, const char *username, const char *passphrase)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_userauth_publickey_auto(file, line, func, session, username, passphrase);
    }
    else
    {
        result = delegate_real_ssh_userauth_publickey_auto(file, line, func, session, username, passphrase);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_userauth_publickey_auto %p, user=%s", (void *)session, username ? username : "(null)");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}
