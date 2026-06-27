#pragma once

/* AsyncProcess の内部実装定義。
 * このヘッダーは startProcess*.cc のみが include する非公開ヘッダー。 */

#include <processController.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* _WIN32 */

#ifndef _WIN32

#include <sys/types.h>

namespace testing {

struct AsyncProcess {
    pid_t pid         = -1;
    int   stdin_fd    = -1;
    int   stdout_fd   = -1;
    int   stderr_fd   = -1;

    /** syslog キャプチャ用パイプ read 端 (-1 = preload_lib 未使用)。 */
    int         debug_log_fd  = -1;
    /** パイプから受信した途中の行バッファ。 */
    std::string debug_log_buf;

    /** waitForExit() が返した終了コード (-1 = 未取得)。 */
    int last_exit_code = -1;

    std::thread             reader_thread;
    std::mutex              buf_mutex;
    std::condition_variable buf_cv;
    std::string             stdout_buf;
    std::string             stderr_buf;
    std::vector<std::string> debug_log_lines;
    bool                    process_done = false;

    AsyncProcess()  = default;
    ~AsyncProcess();

    /* コピー禁止 */
    AsyncProcess(const AsyncProcess&)            = delete;
    AsyncProcess& operator=(const AsyncProcess&) = delete;
};

} // namespace testing

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif /* _WIN32 */

#else /* _WIN32 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef byte
#undef byte
#endif

namespace testing {

struct AsyncProcess {
    HANDLE proc_handle = nullptr;
    HANDLE stdin_h     = nullptr;
    HANDLE stdout_h    = nullptr;
    HANDLE stderr_h    = nullptr;
    DWORD  pid         = 0;

    /** OutputDebugString キャプチャが有効かどうか。 */
    bool capture_debug_output = true;

    /** DBWIN_* 系のシステム シングルトンを巡るテスト プロセス間排他用セマフォ。
     *  capture_debug_output 時に CreateProcess 前に取得し、reader_thread 終了で解放する。 */
    HANDLE dbwin_serialize_sem = nullptr;

    /** DBWIN 受信側のカーネル オブジェクト。CreateProcess より先に組み立てて、
     *  子プロセスの DllMain が OutputDebugStringW を呼んだ瞬間に
     *  DBWIN_BUFFER_READY が必ずシグナル状態になっているようにする。
     *  reader_thread の DBWIN ループ終了時にクローズし、デストラクターは
     *  nullptr フォールバックでのみ解放する。
     *  see: https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringa */
    HANDLE dbwin_buffer_ready = nullptr;
    HANDLE dbwin_data_ready   = nullptr;
    HANDLE dbwin_mapping      = nullptr;
    void  *dbwin_view         = nullptr;

    /** ETW プロバイダ GUID 文字列 (空 = ETW キャプチャ無効)。 */
    std::string etw_provider_guid;
    /** ETW Service フィールドフィルタ (空 = フィルタなし)。 */
    std::string etw_service_filter;

    /** waitForExit() が返した終了コード (-1 = 未取得)。 */
    int last_exit_code = -1;

    std::thread             reader_thread;
    std::mutex              buf_mutex;
    std::condition_variable buf_cv;
    std::string             stdout_buf;
    std::string             stderr_buf;
    std::vector<std::string> debug_log_lines;
    bool                    process_done = false;

    AsyncProcess()  = default;
    ~AsyncProcess();

    /* コピー禁止 */
    AsyncProcess(const AsyncProcess&)            = delete;
    AsyncProcess& operator=(const AsyncProcess&) = delete;
};

} // namespace testing

#endif /* _WIN32 */

namespace testing {
/** OS レベルの stdin 書き込み。トレースなし。writeStdin/writeLineStdin が呼び出す。 */
bool writeStdinImpl(AsyncProcessHandle& handle, const string& data);
} // namespace testing
