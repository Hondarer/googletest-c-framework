#ifndef _MOCK_LIBSSH_H
#define _MOCK_LIBSSH_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* libssh の型定義 (libssh がインストールされていない環境でもモック可能にするため) */
#ifndef SSH_SESSION_STRUCT_DEFINED
#define SSH_SESSION_STRUCT_DEFINED
typedef struct ssh_session_struct *ssh_session;
#endif

#ifndef SSH_CHANNEL_STRUCT_DEFINED
#define SSH_CHANNEL_STRUCT_DEFINED
typedef struct ssh_channel_struct *ssh_channel;
#endif

/* libssh のオプション定義 */
#ifndef SSH_OPTIONS_DEFINED
#define SSH_OPTIONS_DEFINED
enum ssh_options_e {
    SSH_OPTIONS_HOST,
    SSH_OPTIONS_PORT,
    SSH_OPTIONS_PORT_STR,
    SSH_OPTIONS_FD,
    SSH_OPTIONS_USER,
    SSH_OPTIONS_SSH_DIR,
    SSH_OPTIONS_IDENTITY,
    SSH_OPTIONS_ADD_IDENTITY,
    SSH_OPTIONS_KNOWNHOSTS,
    SSH_OPTIONS_TIMEOUT,
    SSH_OPTIONS_TIMEOUT_USEC,
    SSH_OPTIONS_SSH1,
    SSH_OPTIONS_SSH2,
    SSH_OPTIONS_LOG_VERBOSITY,
    SSH_OPTIONS_LOG_VERBOSITY_STR,
    SSH_OPTIONS_CIPHERS_C_S,
    SSH_OPTIONS_CIPHERS_S_C,
    SSH_OPTIONS_COMPRESSION_C_S,
    SSH_OPTIONS_COMPRESSION_S_C,
    SSH_OPTIONS_PROXYCOMMAND,
    SSH_OPTIONS_BINDADDR,
    SSH_OPTIONS_STRICTHOSTKEYCHECK,
    SSH_OPTIONS_COMPRESSION,
    SSH_OPTIONS_COMPRESSION_LEVEL,
    SSH_OPTIONS_KEY_EXCHANGE,
    SSH_OPTIONS_HOSTKEYS,
    SSH_OPTIONS_GSSAPI_SERVER_IDENTITY,
    SSH_OPTIONS_GSSAPI_CLIENT_IDENTITY,
    SSH_OPTIONS_GSSAPI_DELEGATE_CREDENTIALS,
    SSH_OPTIONS_HMAC_C_S,
    SSH_OPTIONS_HMAC_S_C
};
#endif

/* libssh の認証結果定義 */
#ifndef SSH_AUTH_DEFINED
#define SSH_AUTH_DEFINED
enum ssh_auth_e {
    SSH_AUTH_SUCCESS = 0,
    SSH_AUTH_DENIED,
    SSH_AUTH_PARTIAL,
    SSH_AUTH_INFO,
    SSH_AUTH_AGAIN,
    SSH_AUTH_ERROR = -1
};
#endif

/* libssh のエラーコード定義 */
#ifndef SSH_ERROR_DEFINED
#define SSH_ERROR_DEFINED
#define SSH_OK 0
#define SSH_ERROR -1
#define SSH_AGAIN -2
#define SSH_EOF -127
#endif

    /* mock 関数宣言 */
    /* セッション管理 */
    extern ssh_session mock_ssh_new(const char *, const int, const char *);
    extern void mock_ssh_free(const char *, const int, const char *, ssh_session);
    extern int mock_ssh_connect(const char *, const int, const char *, ssh_session);
    extern void mock_ssh_disconnect(const char *, const int, const char *, ssh_session);

    /* オプション設定 */
    extern int mock_ssh_options_set(const char *, const int, const char *, ssh_session, enum ssh_options_e, const void *);

    /* 認証 */
    extern int mock_ssh_userauth_password(const char *, const int, const char *, ssh_session, const char *, const char *);
    extern int mock_ssh_userauth_publickey_auto(const char *, const int, const char *, ssh_session, const char *, const char *);

    /* チャンネル操作 */
    extern ssh_channel mock_ssh_channel_new(const char *, const int, const char *, ssh_session);
    extern void mock_ssh_channel_free(const char *, const int, const char *, ssh_channel);
    extern int mock_ssh_channel_open_session(const char *, const int, const char *, ssh_channel);
    extern int mock_ssh_channel_close(const char *, const int, const char *, ssh_channel);
    extern int mock_ssh_channel_request_exec(const char *, const int, const char *, ssh_channel, const char *);
    extern int mock_ssh_channel_read(const char *, const int, const char *, ssh_channel, void *, uint32_t, int);
    extern int mock_ssh_channel_write(const char *, const int, const char *, ssh_channel, const void *, uint32_t);
    extern int mock_ssh_channel_send_eof(const char *, const int, const char *, ssh_channel);
    extern int mock_ssh_channel_is_eof(const char *, const int, const char *, ssh_channel);

    /* エラー処理 */
    extern const char *mock_ssh_get_error(const char *, const int, const char *, void *);
    extern int mock_ssh_get_error_code(const char *, const int, const char *, void *);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_LIBSSH_H

/* C コード用: マクロで関数置き換え */
#define ssh_new() mock_ssh_new(__FILE__, __LINE__, __func__)
#define ssh_free(session) mock_ssh_free(__FILE__, __LINE__, __func__, session)
#define ssh_connect(session) mock_ssh_connect(__FILE__, __LINE__, __func__, session)
#define ssh_disconnect(session) mock_ssh_disconnect(__FILE__, __LINE__, __func__, session)

#define ssh_options_set(session, type, value) mock_ssh_options_set(__FILE__, __LINE__, __func__, session, type, value)

#define ssh_userauth_password(session, username, password) mock_ssh_userauth_password(__FILE__, __LINE__, __func__, session, username, password)
#define ssh_userauth_publickey_auto(session, username, passphrase) mock_ssh_userauth_publickey_auto(__FILE__, __LINE__, __func__, session, username, passphrase)

#define ssh_channel_new(session) mock_ssh_channel_new(__FILE__, __LINE__, __func__, session)
#define ssh_channel_free(channel) mock_ssh_channel_free(__FILE__, __LINE__, __func__, channel)
#define ssh_channel_open_session(channel) mock_ssh_channel_open_session(__FILE__, __LINE__, __func__, channel)
#define ssh_channel_close(channel) mock_ssh_channel_close(__FILE__, __LINE__, __func__, channel)
#define ssh_channel_request_exec(channel, cmd) mock_ssh_channel_request_exec(__FILE__, __LINE__, __func__, channel, cmd)
#define ssh_channel_read(channel, dest, count, is_stderr) mock_ssh_channel_read(__FILE__, __LINE__, __func__, channel, dest, count, is_stderr)
#define ssh_channel_write(channel, data, len) mock_ssh_channel_write(__FILE__, __LINE__, __func__, channel, data, len)
#define ssh_channel_send_eof(channel) mock_ssh_channel_send_eof(__FILE__, __LINE__, __func__, channel)
#define ssh_channel_is_eof(channel) mock_ssh_channel_is_eof(__FILE__, __LINE__, __func__, channel)

#define ssh_get_error(error) mock_ssh_get_error(__FILE__, __LINE__, __func__, error)
#define ssh_get_error_code(error) mock_ssh_get_error_code(__FILE__, __LINE__, __func__, error)

#else // _IN_OVERRIDE_HEADER_LIBSSH_H

/* C++ テストコード用: Google Mock クラス定義 */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

/* delegate 関数宣言 - セッション管理 */
extern ssh_session delegate_real_ssh_new(const char *, const int, const char *);
extern ssh_session delegate_fake_ssh_new(const char *, const int, const char *);
extern void delegate_real_ssh_free(const char *, const int, const char *, ssh_session);
extern void delegate_fake_ssh_free(const char *, const int, const char *, ssh_session);
extern int delegate_real_ssh_connect(const char *, const int, const char *, ssh_session);
extern int delegate_fake_ssh_connect(const char *, const int, const char *, ssh_session);
extern void delegate_real_ssh_disconnect(const char *, const int, const char *, ssh_session);
extern void delegate_fake_ssh_disconnect(const char *, const int, const char *, ssh_session);

/* delegate 関数宣言 - オプション設定 */
extern int delegate_real_ssh_options_set(const char *, const int, const char *, ssh_session, enum ssh_options_e, const void *);
extern int delegate_fake_ssh_options_set(const char *, const int, const char *, ssh_session, enum ssh_options_e, const void *);

/* delegate 関数宣言 - 認証 */
extern int delegate_real_ssh_userauth_password(const char *, const int, const char *, ssh_session, const char *, const char *);
extern int delegate_fake_ssh_userauth_password(const char *, const int, const char *, ssh_session, const char *, const char *);
extern int delegate_real_ssh_userauth_publickey_auto(const char *, const int, const char *, ssh_session, const char *, const char *);
extern int delegate_fake_ssh_userauth_publickey_auto(const char *, const int, const char *, ssh_session, const char *, const char *);

/* delegate 関数宣言 - チャンネル操作 */
extern ssh_channel delegate_real_ssh_channel_new(const char *, const int, const char *, ssh_session);
extern ssh_channel delegate_fake_ssh_channel_new(const char *, const int, const char *, ssh_session);
extern void delegate_real_ssh_channel_free(const char *, const int, const char *, ssh_channel);
extern void delegate_fake_ssh_channel_free(const char *, const int, const char *, ssh_channel);
extern int delegate_real_ssh_channel_open_session(const char *, const int, const char *, ssh_channel);
extern int delegate_fake_ssh_channel_open_session(const char *, const int, const char *, ssh_channel);
extern int delegate_real_ssh_channel_close(const char *, const int, const char *, ssh_channel);
extern int delegate_fake_ssh_channel_close(const char *, const int, const char *, ssh_channel);
extern int delegate_real_ssh_channel_request_exec(const char *, const int, const char *, ssh_channel, const char *);
extern int delegate_fake_ssh_channel_request_exec(const char *, const int, const char *, ssh_channel, const char *);
extern int delegate_real_ssh_channel_read(const char *, const int, const char *, ssh_channel, void *, uint32_t, int);
extern int delegate_fake_ssh_channel_read(const char *, const int, const char *, ssh_channel, void *, uint32_t, int);
extern int delegate_real_ssh_channel_write(const char *, const int, const char *, ssh_channel, const void *, uint32_t);
extern int delegate_fake_ssh_channel_write(const char *, const int, const char *, ssh_channel, const void *, uint32_t);
extern int delegate_real_ssh_channel_send_eof(const char *, const int, const char *, ssh_channel);
extern int delegate_fake_ssh_channel_send_eof(const char *, const int, const char *, ssh_channel);
extern int delegate_real_ssh_channel_is_eof(const char *, const int, const char *, ssh_channel);
extern int delegate_fake_ssh_channel_is_eof(const char *, const int, const char *, ssh_channel);

/* delegate 関数宣言 - エラー処理 */
extern const char *delegate_real_ssh_get_error(const char *, const int, const char *, void *);
extern const char *delegate_fake_ssh_get_error(const char *, const int, const char *, void *);
extern int delegate_real_ssh_get_error_code(const char *, const int, const char *, void *);
extern int delegate_fake_ssh_get_error_code(const char *, const int, const char *, void *);

class Mock_libssh
{
public:
    /* セッション管理 */
    MOCK_METHOD(ssh_session, ssh_new, (const char *, const int, const char *));
    MOCK_METHOD(void, ssh_free, (const char *, const int, const char *, ssh_session));
    MOCK_METHOD(int, ssh_connect, (const char *, const int, const char *, ssh_session));
    MOCK_METHOD(void, ssh_disconnect, (const char *, const int, const char *, ssh_session));

    /* オプション設定 */
    MOCK_METHOD(int, ssh_options_set, (const char *, const int, const char *, ssh_session, enum ssh_options_e, const void *));

    /* 認証 */
    MOCK_METHOD(int, ssh_userauth_password, (const char *, const int, const char *, ssh_session, const char *, const char *));
    MOCK_METHOD(int, ssh_userauth_publickey_auto, (const char *, const int, const char *, ssh_session, const char *, const char *));

    /* チャンネル操作 */
    MOCK_METHOD(ssh_channel, ssh_channel_new, (const char *, const int, const char *, ssh_session));
    MOCK_METHOD(void, ssh_channel_free, (const char *, const int, const char *, ssh_channel));
    MOCK_METHOD(int, ssh_channel_open_session, (const char *, const int, const char *, ssh_channel));
    MOCK_METHOD(int, ssh_channel_close, (const char *, const int, const char *, ssh_channel));
    MOCK_METHOD(int, ssh_channel_request_exec, (const char *, const int, const char *, ssh_channel, const char *));
    MOCK_METHOD(int, ssh_channel_read, (const char *, const int, const char *, ssh_channel, void *, uint32_t, int));
    MOCK_METHOD(int, ssh_channel_write, (const char *, const int, const char *, ssh_channel, const void *, uint32_t));
    MOCK_METHOD(int, ssh_channel_send_eof, (const char *, const int, const char *, ssh_channel));
    MOCK_METHOD(int, ssh_channel_is_eof, (const char *, const int, const char *, ssh_channel));

    /* エラー処理 */
    MOCK_METHOD(const char *, ssh_get_error, (const char *, const int, const char *, void *));
    MOCK_METHOD(int, ssh_get_error_code, (const char *, const int, const char *, void *));

    void switch_to_real_libssh();
    void switch_to_mock_libssh();

    Mock_libssh();
    ~Mock_libssh();
};

extern Mock_libssh *_mock_libssh;

#endif // _IN_OVERRIDE_HEADER_LIBSSH_H

#endif // _MOCK_LIBSSH_H
