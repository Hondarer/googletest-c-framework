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

    /* エラー処理 */
    ON_CALL(*this, ssh_get_error(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_get_error));

    ON_CALL(*this, ssh_get_error_code(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_ssh_get_error_code));
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

    /* エラー処理 */
    ON_CALL(*this, ssh_get_error(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_get_error));

    ON_CALL(*this, ssh_get_error_code(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_ssh_get_error_code));
}

Mock_libssh::~Mock_libssh()
{
    _mock_libssh = nullptr;
}
