#pragma once

/* AsyncProcess の内部実装定義。
 * このヘッダーは startProcess*.cc のみが include する非公開ヘッダー。 */

#include <startProcess.h>
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

    /** waitProcess() が返した終了コード (-1 = 未取得)。 */
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

    /** waitProcess() が返した終了コード (-1 = 未取得)。 */
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
