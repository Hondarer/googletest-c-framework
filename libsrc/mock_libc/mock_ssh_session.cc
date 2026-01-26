#include <test_com.h>
#include <mock_libssh.h>

#include <stdlib.h>

using namespace testing;

/* ========================================
 * ssh_new
 * ======================================== */

ssh_session delegate_fake_ssh_new(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;

    /* ダミーのセッションポインタを返す */
    ssh_session session = (ssh_session)malloc(sizeof(struct ssh_session_struct *));
    return session;
}

ssh_session delegate_real_ssh_new(const char *file, const int line, const char *func)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_new();
#else
    /* libssh がインストールされていない場合はフェイクを返す */
    return delegate_fake_ssh_new(file, line, func);
#endif
}

ssh_session mock_ssh_new(const char *file, const int line, const char *func)
{
    ssh_session session;

    if (_mock_libssh != nullptr)
    {
        session = _mock_libssh->ssh_new(file, line, func);
    }
    else
    {
        session = delegate_real_ssh_new(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_new");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (session == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> %p\n", file, line, (void *)session);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return session;
}

/* ========================================
 * ssh_free
 * ======================================== */

void delegate_fake_ssh_free(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

    if (session != NULL)
    {
        free(session);
    }
}

void delegate_real_ssh_free(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    ssh_free(session);
#else
    delegate_fake_ssh_free(file, line, func, session);
#endif
}

void mock_ssh_free(const char *file, const int line, const char *func, ssh_session session)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->ssh_free(file, line, func, session);
    }
    else
    {
        delegate_real_ssh_free(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_free %p", (void *)session);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}

/* ========================================
 * ssh_connect
 * ======================================== */

int delegate_fake_ssh_connect(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    /* 成功を返す */
    return SSH_OK;
}

int delegate_real_ssh_connect(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_connect(session);
#else
    return delegate_fake_ssh_connect(file, line, func, session);
#endif
}

int mock_ssh_connect(const char *file, const int line, const char *func, ssh_session session)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_connect(file, line, func, session);
    }
    else
    {
        result = delegate_real_ssh_connect(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_connect %p", (void *)session);
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
 * ssh_disconnect
 * ======================================== */

void delegate_fake_ssh_disconnect(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    /* 何もしない */
}

void delegate_real_ssh_disconnect(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    ssh_disconnect(session);
#else
    delegate_fake_ssh_disconnect(file, line, func, session);
#endif
}

void mock_ssh_disconnect(const char *file, const int line, const char *func, ssh_session session)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->ssh_disconnect(file, line, func, session);
    }
    else
    {
        delegate_real_ssh_disconnect(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_disconnect %p", (void *)session);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}

/* ========================================
 * ssh_options_set
 * ======================================== */

int delegate_fake_ssh_options_set(const char *file, const int line, const char *func,
                                   ssh_session session, enum ssh_options_e type, const void *value)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;
    (void)type;
    (void)value;

    /* 成功を返す */
    return SSH_OK;
}

int delegate_real_ssh_options_set(const char *file, const int line, const char *func,
                                   ssh_session session, enum ssh_options_e type, const void *value)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_options_set(session, type, value);
#else
    return delegate_fake_ssh_options_set(file, line, func, session, type, value);
#endif
}

int mock_ssh_options_set(const char *file, const int line, const char *func,
                          ssh_session session, enum ssh_options_e type, const void *value)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_options_set(file, line, func, session, type, value);
    }
    else
    {
        result = delegate_real_ssh_options_set(file, line, func, session, type, value);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_options_set %p, type=%d", (void *)session, (int)type);
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
