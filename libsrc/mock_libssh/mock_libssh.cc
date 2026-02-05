#include <mock_libssh.h>

using namespace testing;

Mock_libssh *_mock_libssh = nullptr;

Mock_libssh::Mock_libssh()
{
    switch_to_mock_libssh();

    _mock_libssh = this;
}

void Mock_libssh::switch_to_mock_libssh()
{
    /* セッション管理 */
    ON_CALL(*this, ssh_new(_, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_new));

    ON_CALL(*this, ssh_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_free));

    ON_CALL(*this, ssh_connect(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_connect));

    ON_CALL(*this, ssh_disconnect(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_disconnect));

    /* オプション設定 */
    ON_CALL(*this, ssh_options_set(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_options_set));

    /* 認証 */
    ON_CALL(*this, ssh_userauth_password(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_userauth_password));

    ON_CALL(*this, ssh_userauth_publickey_auto(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_userauth_publickey_auto));

    /* チャンネル操作 */
    ON_CALL(*this, ssh_channel_new(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_new));

    ON_CALL(*this, ssh_channel_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_free));

    ON_CALL(*this, ssh_channel_open_session(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_open_session));

    ON_CALL(*this, ssh_channel_close(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_close));

    ON_CALL(*this, ssh_channel_request_exec(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_request_exec));

    ON_CALL(*this, ssh_channel_read(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_read));

    ON_CALL(*this, ssh_channel_write(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_write));

    ON_CALL(*this, ssh_channel_send_eof(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_send_eof));

    ON_CALL(*this, ssh_channel_is_eof(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_channel_is_eof));

    /* 鍵・ホスト検証 */
    ON_CALL(*this, ssh_get_server_publickey(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_get_server_publickey));

    ON_CALL(*this, ssh_get_publickey_hash(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_get_publickey_hash));

    ON_CALL(*this, ssh_key_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_key_free));

    ON_CALL(*this, ssh_session_is_known_server(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_session_is_known_server));

    ON_CALL(*this, ssh_clean_pubkey_hash(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_clean_pubkey_hash));

    ON_CALL(*this, ssh_session_update_known_hosts(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_session_update_known_hosts));

    ON_CALL(*this, ssh_print_hash(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_print_hash));

    /* エラー処理 */
    ON_CALL(*this, ssh_get_error(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_get_error));

    ON_CALL(*this, ssh_get_error_code(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_get_error_code));

    /* SFTP セッション管理 */
    ON_CALL(*this, sftp_new(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_new));

    ON_CALL(*this, sftp_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_free));

    ON_CALL(*this, sftp_init(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_init));

    ON_CALL(*this, sftp_get_error(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_get_error));

    /* SFTP ファイル操作 */
    ON_CALL(*this, sftp_open(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_open));

    ON_CALL(*this, sftp_close(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_close));

    ON_CALL(*this, sftp_read(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_read));

    ON_CALL(*this, sftp_write(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_write));

    ON_CALL(*this, sftp_seek(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_seek));

    ON_CALL(*this, sftp_seek64(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_seek64));

    ON_CALL(*this, sftp_tell(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_tell));

    ON_CALL(*this, sftp_tell64(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_tell64));

    ON_CALL(*this, sftp_rewind(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_rewind));

    ON_CALL(*this, sftp_fstat(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_fstat));

    /* SFTP ディレクトリ操作 */
    ON_CALL(*this, sftp_opendir(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_opendir));

    ON_CALL(*this, sftp_readdir(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_readdir));

    ON_CALL(*this, sftp_closedir(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_closedir));

    ON_CALL(*this, sftp_mkdir(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_mkdir));

    ON_CALL(*this, sftp_rmdir(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_rmdir));

    /* SFTP ファイルシステム操作 */
    ON_CALL(*this, sftp_unlink(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_unlink));

    ON_CALL(*this, sftp_rename(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_rename));

    ON_CALL(*this, sftp_stat(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_stat));

    ON_CALL(*this, sftp_lstat(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_lstat));

    ON_CALL(*this, sftp_attributes_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_sftp_attributes_free));
}

void Mock_libssh::switch_to_real_libssh()
{
    /* セッション管理 */
    ON_CALL(*this, ssh_new(_, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_new));

    ON_CALL(*this, ssh_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_free));

    ON_CALL(*this, ssh_connect(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_connect));

    ON_CALL(*this, ssh_disconnect(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_disconnect));

    /* オプション設定 */
    ON_CALL(*this, ssh_options_set(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_options_set));

    /* 認証 */
    ON_CALL(*this, ssh_userauth_password(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_userauth_password));

    ON_CALL(*this, ssh_userauth_publickey_auto(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_userauth_publickey_auto));

    /* チャンネル操作 */
    ON_CALL(*this, ssh_channel_new(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_new));

    ON_CALL(*this, ssh_channel_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_free));

    ON_CALL(*this, ssh_channel_open_session(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_open_session));

    ON_CALL(*this, ssh_channel_close(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_close));

    ON_CALL(*this, ssh_channel_request_exec(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_request_exec));

    ON_CALL(*this, ssh_channel_read(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_read));

    ON_CALL(*this, ssh_channel_write(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_write));

    ON_CALL(*this, ssh_channel_send_eof(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_send_eof));

    ON_CALL(*this, ssh_channel_is_eof(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_channel_is_eof));

    /* 鍵・ホスト検証 */
    ON_CALL(*this, ssh_get_server_publickey(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_get_server_publickey));

    ON_CALL(*this, ssh_get_publickey_hash(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_get_publickey_hash));

    ON_CALL(*this, ssh_key_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_key_free));

    ON_CALL(*this, ssh_session_is_known_server(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_session_is_known_server));

    ON_CALL(*this, ssh_clean_pubkey_hash(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_clean_pubkey_hash));

    ON_CALL(*this, ssh_session_update_known_hosts(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_session_update_known_hosts));

    ON_CALL(*this, ssh_print_hash(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_print_hash));

    /* エラー処理 */
    ON_CALL(*this, ssh_get_error(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_get_error));

    ON_CALL(*this, ssh_get_error_code(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_get_error_code));

    /* SFTP セッション管理 */
    ON_CALL(*this, sftp_new(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_new));

    ON_CALL(*this, sftp_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_free));

    ON_CALL(*this, sftp_init(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_init));

    ON_CALL(*this, sftp_get_error(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_get_error));

    /* SFTP ファイル操作 */
    ON_CALL(*this, sftp_open(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_open));

    ON_CALL(*this, sftp_close(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_close));

    ON_CALL(*this, sftp_read(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_read));

    ON_CALL(*this, sftp_write(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_write));

    ON_CALL(*this, sftp_seek(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_seek));

    ON_CALL(*this, sftp_seek64(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_seek64));

    ON_CALL(*this, sftp_tell(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_tell));

    ON_CALL(*this, sftp_tell64(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_tell64));

    ON_CALL(*this, sftp_rewind(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_rewind));

    ON_CALL(*this, sftp_fstat(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_fstat));

    /* SFTP ディレクトリ操作 */
    ON_CALL(*this, sftp_opendir(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_opendir));

    ON_CALL(*this, sftp_readdir(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_readdir));

    ON_CALL(*this, sftp_closedir(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_closedir));

    ON_CALL(*this, sftp_mkdir(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_mkdir));

    ON_CALL(*this, sftp_rmdir(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_rmdir));

    /* SFTP ファイルシステム操作 */
    ON_CALL(*this, sftp_unlink(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_unlink));

    ON_CALL(*this, sftp_rename(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_rename));

    ON_CALL(*this, sftp_stat(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_stat));

    ON_CALL(*this, sftp_lstat(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_lstat));

    ON_CALL(*this, sftp_attributes_free(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sftp_attributes_free));
}

Mock_libssh::~Mock_libssh()
{
    _mock_libssh = nullptr;
}
