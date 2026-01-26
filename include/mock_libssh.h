#ifndef _MOCK_LIBSSH_H
#define _MOCK_LIBSSH_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

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

/* SFTP の型定義 */
#ifndef SFTP_SESSION_STRUCT_DEFINED
#define SFTP_SESSION_STRUCT_DEFINED
typedef struct sftp_session_struct *sftp_session;
#endif

#ifndef SFTP_FILE_STRUCT_DEFINED
#define SFTP_FILE_STRUCT_DEFINED
typedef struct sftp_file_struct *sftp_file;
#endif

#ifndef SFTP_DIR_STRUCT_DEFINED
#define SFTP_DIR_STRUCT_DEFINED
typedef struct sftp_dir_struct *sftp_dir;
#endif

#ifndef SFTP_ATTRIBUTES_STRUCT_DEFINED
#define SFTP_ATTRIBUTES_STRUCT_DEFINED
typedef struct sftp_attributes_struct *sftp_attributes;

/* sftp_attributes 構造体 (簡易版) */
struct sftp_attributes_struct {
    char *name;
    char *longname;
    uint32_t flags;
    uint8_t type;
    uint64_t size;
    uint32_t uid;
    uint32_t gid;
    char *owner;
    char *group;
    uint32_t permissions;
    uint64_t atime64;
    uint32_t atime;
    uint32_t atime_nseconds;
    uint64_t createtime;
    uint32_t createtime_nseconds;
    uint64_t mtime64;
    uint32_t mtime;
    uint32_t mtime_nseconds;
    char *acl;
    uint32_t extended_count;
    char *extended_type;
    char *extended_data;
};
#endif

/* SFTP ファイルタイプ定義 */
#ifndef SFTP_TYPE_DEFINED
#define SFTP_TYPE_DEFINED
#define SSH_FILEXFER_TYPE_REGULAR          1
#define SSH_FILEXFER_TYPE_DIRECTORY        2
#define SSH_FILEXFER_TYPE_SYMLINK          3
#define SSH_FILEXFER_TYPE_SPECIAL          4
#define SSH_FILEXFER_TYPE_UNKNOWN          5
#endif

/* SFTP アクセスフラグ定義 */
#ifndef SFTP_FLAGS_DEFINED
#define SFTP_FLAGS_DEFINED
#define O_RDONLY    0x0000
#define O_WRONLY    0x0001
#define O_RDWR      0x0002
#define O_CREAT     0x0100
#define O_EXCL      0x0200
#define O_TRUNC     0x1000
#define O_APPEND    0x2000
#endif

/* SFTP エラーコード定義 */
#ifndef SFTP_ERROR_DEFINED
#define SFTP_ERROR_DEFINED
#define SSH_FX_OK                            0
#define SSH_FX_EOF                           1
#define SSH_FX_NO_SUCH_FILE                  2
#define SSH_FX_PERMISSION_DENIED             3
#define SSH_FX_FAILURE                       4
#define SSH_FX_BAD_MESSAGE                   5
#define SSH_FX_NO_CONNECTION                 6
#define SSH_FX_CONNECTION_LOST               7
#define SSH_FX_OP_UNSUPPORTED                8
#define SSH_FX_INVALID_HANDLE                9
#define SSH_FX_NO_SUCH_PATH                  10
#define SSH_FX_FILE_ALREADY_EXISTS           11
#define SSH_FX_WRITE_PROTECT                 12
#define SSH_FX_NO_MEDIA                      13
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

    /* SFTP セッション管理 */
    extern sftp_session mock_sftp_new(const char *, const int, const char *, ssh_session);
    extern void mock_sftp_free(const char *, const int, const char *, sftp_session);
    extern int mock_sftp_init(const char *, const int, const char *, sftp_session);
    extern int mock_sftp_get_error(const char *, const int, const char *, sftp_session);

    /* SFTP ファイル操作 */
    extern sftp_file mock_sftp_open(const char *, const int, const char *, sftp_session, const char *, int, mode_t);
    extern int mock_sftp_close(const char *, const int, const char *, sftp_file);
    extern ssize_t mock_sftp_read(const char *, const int, const char *, sftp_file, void *, size_t);
    extern ssize_t mock_sftp_write(const char *, const int, const char *, sftp_file, const void *, size_t);
    extern int mock_sftp_seek(const char *, const int, const char *, sftp_file, uint32_t);
    extern int mock_sftp_seek64(const char *, const int, const char *, sftp_file, uint64_t);
    extern unsigned long mock_sftp_tell(const char *, const int, const char *, sftp_file);
    extern uint64_t mock_sftp_tell64(const char *, const int, const char *, sftp_file);
    extern void mock_sftp_rewind(const char *, const int, const char *, sftp_file);
    extern sftp_attributes mock_sftp_fstat(const char *, const int, const char *, sftp_file);

    /* SFTP ディレクトリ操作 */
    extern sftp_dir mock_sftp_opendir(const char *, const int, const char *, sftp_session, const char *);
    extern sftp_attributes mock_sftp_readdir(const char *, const int, const char *, sftp_session, sftp_dir);
    extern int mock_sftp_closedir(const char *, const int, const char *, sftp_dir);
    extern int mock_sftp_mkdir(const char *, const int, const char *, sftp_session, const char *, mode_t);
    extern int mock_sftp_rmdir(const char *, const int, const char *, sftp_session, const char *);

    /* SFTP ファイルシステム操作 */
    extern int mock_sftp_unlink(const char *, const int, const char *, sftp_session, const char *);
    extern int mock_sftp_rename(const char *, const int, const char *, sftp_session, const char *, const char *);
    extern sftp_attributes mock_sftp_stat(const char *, const int, const char *, sftp_session, const char *);
    extern sftp_attributes mock_sftp_lstat(const char *, const int, const char *, sftp_session, const char *);
    extern void mock_sftp_attributes_free(const char *, const int, const char *, sftp_attributes);

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

/* SFTP セッション管理 */
#define sftp_new(session) mock_sftp_new(__FILE__, __LINE__, __func__, session)
#define sftp_free(sftp) mock_sftp_free(__FILE__, __LINE__, __func__, sftp)
#define sftp_init(sftp) mock_sftp_init(__FILE__, __LINE__, __func__, sftp)
#define sftp_get_error(sftp) mock_sftp_get_error(__FILE__, __LINE__, __func__, sftp)

/* SFTP ファイル操作 */
#define sftp_open(sftp, file, accesstype, mode) mock_sftp_open(__FILE__, __LINE__, __func__, sftp, file, accesstype, mode)
#define sftp_close(file) mock_sftp_close(__FILE__, __LINE__, __func__, file)
#define sftp_read(file, buf, count) mock_sftp_read(__FILE__, __LINE__, __func__, file, buf, count)
#define sftp_write(file, buf, count) mock_sftp_write(__FILE__, __LINE__, __func__, file, buf, count)
#define sftp_seek(file, new_offset) mock_sftp_seek(__FILE__, __LINE__, __func__, file, new_offset)
#define sftp_seek64(file, new_offset) mock_sftp_seek64(__FILE__, __LINE__, __func__, file, new_offset)
#define sftp_tell(file) mock_sftp_tell(__FILE__, __LINE__, __func__, file)
#define sftp_tell64(file) mock_sftp_tell64(__FILE__, __LINE__, __func__, file)
#define sftp_rewind(file) mock_sftp_rewind(__FILE__, __LINE__, __func__, file)
#define sftp_fstat(file) mock_sftp_fstat(__FILE__, __LINE__, __func__, file)

/* SFTP ディレクトリ操作 */
#define sftp_opendir(sftp, path) mock_sftp_opendir(__FILE__, __LINE__, __func__, sftp, path)
#define sftp_readdir(sftp, dir) mock_sftp_readdir(__FILE__, __LINE__, __func__, sftp, dir)
#define sftp_closedir(dir) mock_sftp_closedir(__FILE__, __LINE__, __func__, dir)
#define sftp_mkdir(sftp, directory, mode) mock_sftp_mkdir(__FILE__, __LINE__, __func__, sftp, directory, mode)
#define sftp_rmdir(sftp, directory) mock_sftp_rmdir(__FILE__, __LINE__, __func__, sftp, directory)

/* SFTP ファイルシステム操作 */
#define sftp_unlink(sftp, file) mock_sftp_unlink(__FILE__, __LINE__, __func__, sftp, file)
#define sftp_rename(sftp, original, newname) mock_sftp_rename(__FILE__, __LINE__, __func__, sftp, original, newname)
#define sftp_stat(sftp, path) mock_sftp_stat(__FILE__, __LINE__, __func__, sftp, path)
#define sftp_lstat(sftp, path) mock_sftp_lstat(__FILE__, __LINE__, __func__, sftp, path)
#define sftp_attributes_free(attr) mock_sftp_attributes_free(__FILE__, __LINE__, __func__, attr)

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

/* delegate 関数宣言 - SFTP セッション管理 */
extern sftp_session delegate_real_sftp_new(const char *, const int, const char *, ssh_session);
extern sftp_session delegate_fake_sftp_new(const char *, const int, const char *, ssh_session);
extern void delegate_real_sftp_free(const char *, const int, const char *, sftp_session);
extern void delegate_fake_sftp_free(const char *, const int, const char *, sftp_session);
extern int delegate_real_sftp_init(const char *, const int, const char *, sftp_session);
extern int delegate_fake_sftp_init(const char *, const int, const char *, sftp_session);
extern int delegate_real_sftp_get_error(const char *, const int, const char *, sftp_session);
extern int delegate_fake_sftp_get_error(const char *, const int, const char *, sftp_session);

/* delegate 関数宣言 - SFTP ファイル操作 */
extern sftp_file delegate_real_sftp_open(const char *, const int, const char *, sftp_session, const char *, int, mode_t);
extern sftp_file delegate_fake_sftp_open(const char *, const int, const char *, sftp_session, const char *, int, mode_t);
extern int delegate_real_sftp_close(const char *, const int, const char *, sftp_file);
extern int delegate_fake_sftp_close(const char *, const int, const char *, sftp_file);
extern ssize_t delegate_real_sftp_read(const char *, const int, const char *, sftp_file, void *, size_t);
extern ssize_t delegate_fake_sftp_read(const char *, const int, const char *, sftp_file, void *, size_t);
extern ssize_t delegate_real_sftp_write(const char *, const int, const char *, sftp_file, const void *, size_t);
extern ssize_t delegate_fake_sftp_write(const char *, const int, const char *, sftp_file, const void *, size_t);
extern int delegate_real_sftp_seek(const char *, const int, const char *, sftp_file, uint32_t);
extern int delegate_fake_sftp_seek(const char *, const int, const char *, sftp_file, uint32_t);
extern int delegate_real_sftp_seek64(const char *, const int, const char *, sftp_file, uint64_t);
extern int delegate_fake_sftp_seek64(const char *, const int, const char *, sftp_file, uint64_t);
extern unsigned long delegate_real_sftp_tell(const char *, const int, const char *, sftp_file);
extern unsigned long delegate_fake_sftp_tell(const char *, const int, const char *, sftp_file);
extern uint64_t delegate_real_sftp_tell64(const char *, const int, const char *, sftp_file);
extern uint64_t delegate_fake_sftp_tell64(const char *, const int, const char *, sftp_file);
extern void delegate_real_sftp_rewind(const char *, const int, const char *, sftp_file);
extern void delegate_fake_sftp_rewind(const char *, const int, const char *, sftp_file);
extern sftp_attributes delegate_real_sftp_fstat(const char *, const int, const char *, sftp_file);
extern sftp_attributes delegate_fake_sftp_fstat(const char *, const int, const char *, sftp_file);

/* delegate 関数宣言 - SFTP ディレクトリ操作 */
extern sftp_dir delegate_real_sftp_opendir(const char *, const int, const char *, sftp_session, const char *);
extern sftp_dir delegate_fake_sftp_opendir(const char *, const int, const char *, sftp_session, const char *);
extern sftp_attributes delegate_real_sftp_readdir(const char *, const int, const char *, sftp_session, sftp_dir);
extern sftp_attributes delegate_fake_sftp_readdir(const char *, const int, const char *, sftp_session, sftp_dir);
extern int delegate_real_sftp_closedir(const char *, const int, const char *, sftp_dir);
extern int delegate_fake_sftp_closedir(const char *, const int, const char *, sftp_dir);
extern int delegate_real_sftp_mkdir(const char *, const int, const char *, sftp_session, const char *, mode_t);
extern int delegate_fake_sftp_mkdir(const char *, const int, const char *, sftp_session, const char *, mode_t);
extern int delegate_real_sftp_rmdir(const char *, const int, const char *, sftp_session, const char *);
extern int delegate_fake_sftp_rmdir(const char *, const int, const char *, sftp_session, const char *);

/* delegate 関数宣言 - SFTP ファイルシステム操作 */
extern int delegate_real_sftp_unlink(const char *, const int, const char *, sftp_session, const char *);
extern int delegate_fake_sftp_unlink(const char *, const int, const char *, sftp_session, const char *);
extern int delegate_real_sftp_rename(const char *, const int, const char *, sftp_session, const char *, const char *);
extern int delegate_fake_sftp_rename(const char *, const int, const char *, sftp_session, const char *, const char *);
extern sftp_attributes delegate_real_sftp_stat(const char *, const int, const char *, sftp_session, const char *);
extern sftp_attributes delegate_fake_sftp_stat(const char *, const int, const char *, sftp_session, const char *);
extern sftp_attributes delegate_real_sftp_lstat(const char *, const int, const char *, sftp_session, const char *);
extern sftp_attributes delegate_fake_sftp_lstat(const char *, const int, const char *, sftp_session, const char *);
extern void delegate_real_sftp_attributes_free(const char *, const int, const char *, sftp_attributes);
extern void delegate_fake_sftp_attributes_free(const char *, const int, const char *, sftp_attributes);

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

    /* SFTP セッション管理 */
    MOCK_METHOD(sftp_session, sftp_new, (const char *, const int, const char *, ssh_session));
    MOCK_METHOD(void, sftp_free, (const char *, const int, const char *, sftp_session));
    MOCK_METHOD(int, sftp_init, (const char *, const int, const char *, sftp_session));
    MOCK_METHOD(int, sftp_get_error, (const char *, const int, const char *, sftp_session));

    /* SFTP ファイル操作 */
    MOCK_METHOD(sftp_file, sftp_open, (const char *, const int, const char *, sftp_session, const char *, int, mode_t));
    MOCK_METHOD(int, sftp_close, (const char *, const int, const char *, sftp_file));
    MOCK_METHOD(ssize_t, sftp_read, (const char *, const int, const char *, sftp_file, void *, size_t));
    MOCK_METHOD(ssize_t, sftp_write, (const char *, const int, const char *, sftp_file, const void *, size_t));
    MOCK_METHOD(int, sftp_seek, (const char *, const int, const char *, sftp_file, uint32_t));
    MOCK_METHOD(int, sftp_seek64, (const char *, const int, const char *, sftp_file, uint64_t));
    MOCK_METHOD(unsigned long, sftp_tell, (const char *, const int, const char *, sftp_file));
    MOCK_METHOD(uint64_t, sftp_tell64, (const char *, const int, const char *, sftp_file));
    MOCK_METHOD(void, sftp_rewind, (const char *, const int, const char *, sftp_file));
    MOCK_METHOD(sftp_attributes, sftp_fstat, (const char *, const int, const char *, sftp_file));

    /* SFTP ディレクトリ操作 */
    MOCK_METHOD(sftp_dir, sftp_opendir, (const char *, const int, const char *, sftp_session, const char *));
    MOCK_METHOD(sftp_attributes, sftp_readdir, (const char *, const int, const char *, sftp_session, sftp_dir));
    MOCK_METHOD(int, sftp_closedir, (const char *, const int, const char *, sftp_dir));
    MOCK_METHOD(int, sftp_mkdir, (const char *, const int, const char *, sftp_session, const char *, mode_t));
    MOCK_METHOD(int, sftp_rmdir, (const char *, const int, const char *, sftp_session, const char *));

    /* SFTP ファイルシステム操作 */
    MOCK_METHOD(int, sftp_unlink, (const char *, const int, const char *, sftp_session, const char *));
    MOCK_METHOD(int, sftp_rename, (const char *, const int, const char *, sftp_session, const char *, const char *));
    MOCK_METHOD(sftp_attributes, sftp_stat, (const char *, const int, const char *, sftp_session, const char *));
    MOCK_METHOD(sftp_attributes, sftp_lstat, (const char *, const int, const char *, sftp_session, const char *));
    MOCK_METHOD(void, sftp_attributes_free, (const char *, const int, const char *, sftp_attributes));

    void switch_to_real_libssh();
    void switch_to_mock_libssh();

    Mock_libssh();
    ~Mock_libssh();
};

extern Mock_libssh *_mock_libssh;

#endif // _IN_OVERRIDE_HEADER_LIBSSH_H

#endif // _MOCK_LIBSSH_H
