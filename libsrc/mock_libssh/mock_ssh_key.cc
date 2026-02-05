#include <test_com.h>
#include <mock_libssh.h>

#include <stdlib.h>
#include <string.h>

using namespace testing;

/* ========================================
 * ssh_get_server_publickey
 * ======================================== */

int delegate_fake_ssh_get_server_publickey(const char *file, const int line, const char *func,
                                            ssh_session session, ssh_key *key)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    /* ダミーの鍵ポインタを返す */
    if (key != NULL)
    {
        *key = (ssh_key)malloc(sizeof(struct ssh_key_struct *));
    }
    return SSH_OK;
}

int delegate_real_ssh_get_server_publickey(const char *file, const int line, const char *func,
                                            ssh_session session, ssh_key *key)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_get_server_publickey(session, key);
#else
    return delegate_fake_ssh_get_server_publickey(file, line, func, session, key);
#endif
}

int mock_ssh_get_server_publickey(const char *file, const int line, const char *func,
                                   ssh_session session, ssh_key *key)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_get_server_publickey(file, line, func, session, key);
    }
    else
    {
        result = delegate_real_ssh_get_server_publickey(file, line, func, session, key);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_get_server_publickey %p", (void *)session);
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
 * ssh_get_publickey_hash
 * ======================================== */

int delegate_fake_ssh_get_publickey_hash(const char *file, const int line, const char *func,
                                          const ssh_key key, enum ssh_publickey_hash_type type,
                                          unsigned char **hash, size_t *hlen)
{
    (void)file;
    (void)line;
    (void)func;
    (void)key;
    (void)type;

    /* ダミーのハッシュを返す */
    if (hash != NULL && hlen != NULL)
    {
        *hlen = 20;
        *hash = (unsigned char *)malloc(*hlen);
        if (*hash != NULL)
        {
            memset(*hash, 0, *hlen);
        }
    }
    return SSH_OK;
}

int delegate_real_ssh_get_publickey_hash(const char *file, const int line, const char *func,
                                          const ssh_key key, enum ssh_publickey_hash_type type,
                                          unsigned char **hash, size_t *hlen)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_get_publickey_hash(key, type, hash, hlen);
#else
    return delegate_fake_ssh_get_publickey_hash(file, line, func, key, type, hash, hlen);
#endif
}

int mock_ssh_get_publickey_hash(const char *file, const int line, const char *func,
                                 const ssh_key key, enum ssh_publickey_hash_type type,
                                 unsigned char **hash, size_t *hlen)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_get_publickey_hash(file, line, func, key, type, hash, hlen);
    }
    else
    {
        result = delegate_real_ssh_get_publickey_hash(file, line, func, key, type, hash, hlen);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_get_publickey_hash %p, type=%d", (const void *)key, (int)type);
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
 * ssh_key_free
 * ======================================== */

void delegate_fake_ssh_key_free(const char *file, const int line, const char *func, ssh_key key)
{
    (void)file;
    (void)line;
    (void)func;

    if (key != NULL)
    {
        free(key);
    }
}

void delegate_real_ssh_key_free(const char *file, const int line, const char *func, ssh_key key)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    ssh_key_free(key);
#else
    delegate_fake_ssh_key_free(file, line, func, key);
#endif
}

void mock_ssh_key_free(const char *file, const int line, const char *func, ssh_key key)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->ssh_key_free(file, line, func, key);
    }
    else
    {
        delegate_real_ssh_key_free(file, line, func, key);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_key_free %p", (void *)key);
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
 * ssh_session_is_known_server
 * ======================================== */

enum ssh_known_hosts_e delegate_fake_ssh_session_is_known_server(const char *file, const int line, const char *func,
                                                                   ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    /* 既知ホストとして OK を返す */
    return SSH_KNOWN_HOSTS_OK;
}

enum ssh_known_hosts_e delegate_real_ssh_session_is_known_server(const char *file, const int line, const char *func,
                                                                   ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_session_is_known_server(session);
#else
    return delegate_fake_ssh_session_is_known_server(file, line, func, session);
#endif
}

enum ssh_known_hosts_e mock_ssh_session_is_known_server(const char *file, const int line, const char *func,
                                                          ssh_session session)
{
    enum ssh_known_hosts_e result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_session_is_known_server(file, line, func, session);
    }
    else
    {
        result = delegate_real_ssh_session_is_known_server(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_session_is_known_server %p", (void *)session);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, (int)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * ssh_clean_pubkey_hash
 * ======================================== */

void delegate_fake_ssh_clean_pubkey_hash(const char *file, const int line, const char *func,
                                           unsigned char **hash)
{
    (void)file;
    (void)line;
    (void)func;

    if (hash != NULL && *hash != NULL)
    {
        free(*hash);
        *hash = NULL;
    }
}

void delegate_real_ssh_clean_pubkey_hash(const char *file, const int line, const char *func,
                                           unsigned char **hash)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    ssh_clean_pubkey_hash(hash);
#else
    delegate_fake_ssh_clean_pubkey_hash(file, line, func, hash);
#endif
}

void mock_ssh_clean_pubkey_hash(const char *file, const int line, const char *func,
                                  unsigned char **hash)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->ssh_clean_pubkey_hash(file, line, func, hash);
    }
    else
    {
        delegate_real_ssh_clean_pubkey_hash(file, line, func, hash);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_clean_pubkey_hash");
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
 * ssh_session_update_known_hosts
 * ======================================== */

int delegate_fake_ssh_session_update_known_hosts(const char *file, const int line, const char *func,
                                                   ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    /* 成功を返す */
    return SSH_OK;
}

int delegate_real_ssh_session_update_known_hosts(const char *file, const int line, const char *func,
                                                   ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_session_update_known_hosts(session);
#else
    return delegate_fake_ssh_session_update_known_hosts(file, line, func, session);
#endif
}

int mock_ssh_session_update_known_hosts(const char *file, const int line, const char *func,
                                          ssh_session session)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_session_update_known_hosts(file, line, func, session);
    }
    else
    {
        result = delegate_real_ssh_session_update_known_hosts(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_session_update_known_hosts %p", (void *)session);
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
 * ssh_print_hash
 * ======================================== */

void delegate_fake_ssh_print_hash(const char *file, const int line, const char *func,
                                    enum ssh_publickey_hash_type type, unsigned char *hash, size_t len)
{
    (void)file;
    (void)line;
    (void)func;
    (void)type;
    (void)hash;
    (void)len;

    /* 何もしない */
}

void delegate_real_ssh_print_hash(const char *file, const int line, const char *func,
                                    enum ssh_publickey_hash_type type, unsigned char *hash, size_t len)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    ssh_print_hash(type, hash, len);
#else
    delegate_fake_ssh_print_hash(file, line, func, type, hash, len);
#endif
}

void mock_ssh_print_hash(const char *file, const int line, const char *func,
                           enum ssh_publickey_hash_type type, unsigned char *hash, size_t len)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->ssh_print_hash(file, line, func, type, hash, len);
    }
    else
    {
        delegate_real_ssh_print_hash(file, line, func, type, hash, len);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_print_hash type=%d, len=%zu", (int)type, len);
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
