#include <test_com.h>
#include <mock_libssh.h>

#include <stdlib.h>
#include <string.h>

using namespace testing;

/* ========================================
 * ssh_channel_new
 * ======================================== */

ssh_channel delegate_fake_ssh_channel_new(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    /* ダミーのチャンネルポインタを返す */
    ssh_channel channel = (ssh_channel)malloc(sizeof(struct ssh_channel_struct *));
    return channel;
}

ssh_channel delegate_real_ssh_channel_new(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_new(session);
#else
    return delegate_fake_ssh_channel_new(file, line, func, session);
#endif
}

ssh_channel mock_ssh_channel_new(const char *file, const int line, const char *func, ssh_session session)
{
    ssh_channel channel;

    if (_mock_libssh != nullptr)
    {
        channel = _mock_libssh->ssh_channel_new(file, line, func, session);
    }
    else
    {
        channel = delegate_real_ssh_channel_new(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_new %p", (void *)session);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            if (channel == NULL)
            {
                printf(" from %s:%d -> NULL\n", file, line);
            }
            else
            {
                printf(" from %s:%d -> %p\n", file, line, (void *)channel);
            }
        }
        else
        {
            printf("\n");
        }
    }

    return channel;
}

/* ========================================
 * ssh_channel_free
 * ======================================== */

void delegate_fake_ssh_channel_free(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;

    if (channel != NULL)
    {
        free(channel);
    }
}

void delegate_real_ssh_channel_free(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    ssh_channel_free(channel);
#else
    delegate_fake_ssh_channel_free(file, line, func, channel);
#endif
}

void mock_ssh_channel_free(const char *file, const int line, const char *func, ssh_channel channel)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->ssh_channel_free(file, line, func, channel);
    }
    else
    {
        delegate_real_ssh_channel_free(file, line, func, channel);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_free %p", (void *)channel);
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
 * ssh_channel_open_session
 * ======================================== */

int delegate_fake_ssh_channel_open_session(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;

    return SSH_OK;
}

int delegate_real_ssh_channel_open_session(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_open_session(channel);
#else
    return delegate_fake_ssh_channel_open_session(file, line, func, channel);
#endif
}

int mock_ssh_channel_open_session(const char *file, const int line, const char *func, ssh_channel channel)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_open_session(file, line, func, channel);
    }
    else
    {
        result = delegate_real_ssh_channel_open_session(file, line, func, channel);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_open_session %p", (void *)channel);
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
 * ssh_channel_close
 * ======================================== */

int delegate_fake_ssh_channel_close(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;

    return SSH_OK;
}

int delegate_real_ssh_channel_close(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_close(channel);
#else
    return delegate_fake_ssh_channel_close(file, line, func, channel);
#endif
}

int mock_ssh_channel_close(const char *file, const int line, const char *func, ssh_channel channel)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_close(file, line, func, channel);
    }
    else
    {
        result = delegate_real_ssh_channel_close(file, line, func, channel);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_close %p", (void *)channel);
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
 * ssh_channel_request_exec
 * ======================================== */

int delegate_fake_ssh_channel_request_exec(const char *file, const int line, const char *func,
                                            ssh_channel channel, const char *cmd)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;
    (void)cmd;

    return SSH_OK;
}

int delegate_real_ssh_channel_request_exec(const char *file, const int line, const char *func,
                                            ssh_channel channel, const char *cmd)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_request_exec(channel, cmd);
#else
    return delegate_fake_ssh_channel_request_exec(file, line, func, channel, cmd);
#endif
}

int mock_ssh_channel_request_exec(const char *file, const int line, const char *func,
                                   ssh_channel channel, const char *cmd)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_request_exec(file, line, func, channel, cmd);
    }
    else
    {
        result = delegate_real_ssh_channel_request_exec(file, line, func, channel, cmd);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_request_exec %p, cmd=%s", (void *)channel, cmd ? cmd : "(null)");
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
 * ssh_channel_read
 * ======================================== */

int delegate_fake_ssh_channel_read(const char *file, const int line, const char *func,
                                    ssh_channel channel, void *dest, uint32_t count, int is_stderr)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;
    (void)dest;
    (void)count;
    (void)is_stderr;

    /* EOF を返す (読み取るデータがない) */
    return 0;
}

int delegate_real_ssh_channel_read(const char *file, const int line, const char *func,
                                    ssh_channel channel, void *dest, uint32_t count, int is_stderr)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_read(channel, dest, count, is_stderr);
#else
    return delegate_fake_ssh_channel_read(file, line, func, channel, dest, count, is_stderr);
#endif
}

int mock_ssh_channel_read(const char *file, const int line, const char *func,
                           ssh_channel channel, void *dest, uint32_t count, int is_stderr)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_read(file, line, func, channel, dest, count, is_stderr);
    }
    else
    {
        result = delegate_real_ssh_channel_read(file, line, func, channel, dest, count, is_stderr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_read %p, count=%u, is_stderr=%d", (void *)channel, count, is_stderr);
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
 * ssh_channel_write
 * ======================================== */

int delegate_fake_ssh_channel_write(const char *file, const int line, const char *func,
                                     ssh_channel channel, const void *data, uint32_t len)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;
    (void)data;

    /* 書き込んだバイト数を返す */
    return (int)len;
}

int delegate_real_ssh_channel_write(const char *file, const int line, const char *func,
                                     ssh_channel channel, const void *data, uint32_t len)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_write(channel, data, len);
#else
    return delegate_fake_ssh_channel_write(file, line, func, channel, data, len);
#endif
}

int mock_ssh_channel_write(const char *file, const int line, const char *func,
                            ssh_channel channel, const void *data, uint32_t len)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_write(file, line, func, channel, data, len);
    }
    else
    {
        result = delegate_real_ssh_channel_write(file, line, func, channel, data, len);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_write %p, len=%u", (void *)channel, len);
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
 * ssh_channel_send_eof
 * ======================================== */

int delegate_fake_ssh_channel_send_eof(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;

    return SSH_OK;
}

int delegate_real_ssh_channel_send_eof(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_send_eof(channel);
#else
    return delegate_fake_ssh_channel_send_eof(file, line, func, channel);
#endif
}

int mock_ssh_channel_send_eof(const char *file, const int line, const char *func, ssh_channel channel)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_send_eof(file, line, func, channel);
    }
    else
    {
        result = delegate_real_ssh_channel_send_eof(file, line, func, channel);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_send_eof %p", (void *)channel);
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
 * ssh_channel_is_eof
 * ======================================== */

int delegate_fake_ssh_channel_is_eof(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;
    (void)channel;

    /* EOF ではないと返す */
    return 0;
}

int delegate_real_ssh_channel_is_eof(const char *file, const int line, const char *func, ssh_channel channel)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return ssh_channel_is_eof(channel);
#else
    return delegate_fake_ssh_channel_is_eof(file, line, func, channel);
#endif
}

int mock_ssh_channel_is_eof(const char *file, const int line, const char *func, ssh_channel channel)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->ssh_channel_is_eof(file, line, func, channel);
    }
    else
    {
        result = delegate_real_ssh_channel_is_eof(file, line, func, channel);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > ssh_channel_is_eof %p", (void *)channel);
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
