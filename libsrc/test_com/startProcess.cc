/* プラットフォーム共通の非同期プロセス制御 API 実装。
 * プラットフォーム固有の関数 (startProcessAsync / writeStdin / closeStdin / waitProcess /
 * interruptProcess / killProcess) は startProcess_linux.cc / startProcess_windows.cc に実装。 */

#include "startProcess_impl.h"

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

namespace testing {

/* -------- writeLineStdin -------- */

bool writeLineStdin(AsyncProcessHandle& handle, const string& line)
{
    return writeStdin(handle, line + "\n");
}

/* -------- waitForOutput -------- */

string waitForOutput(AsyncProcessHandle& handle,
                     const string& pattern,
                     int timeout_ms)
{
    if (!handle) {
        throw runtime_error("waitForOutput: null handle");
    }

    unique_lock<mutex> lk(handle->buf_mutex);

    auto check = [&]() -> bool {
        return handle->stdout_buf.find(pattern) != string::npos || handle->process_done;
    };

    if (timeout_ms < 0) {
        handle->buf_cv.wait(lk, check);
    } else {
        auto deadline = chrono::steady_clock::now() + chrono::milliseconds(timeout_ms);
        handle->buf_cv.wait_until(lk, deadline, check);
    }

    size_t pos = handle->stdout_buf.find(pattern);
    if (pos == string::npos) {
        throw runtime_error(
            "waitForOutput: timeout or EOF before pattern: \"" + pattern + "\"");
    }

    return handle->stdout_buf.substr(0, pos + pattern.size());
}

/* -------- getStdout -------- */

string getStdout(AsyncProcessHandle& handle)
{
    if (!handle) { return {}; }
    lock_guard<mutex> lk(handle->buf_mutex);
    return handle->stdout_buf;
}

/* -------- getStderr -------- */

string getStderr(AsyncProcessHandle& handle)
{
    if (!handle) { return {}; }
    lock_guard<mutex> lk(handle->buf_mutex);
    return handle->stderr_buf;
}

/* -------- getDebugLogCount -------- */

size_t getDebugLogCount(AsyncProcessHandle& handle)
{
    if (!handle) { return 0; }
    lock_guard<mutex> lk(handle->buf_mutex);
    return handle->debug_log_lines.size();
}

/* -------- getDebugLog -------- */

vector<string> getDebugLog(AsyncProcessHandle& handle, size_t from_index)
{
    if (!handle) { return {}; }
    lock_guard<mutex> lk(handle->buf_mutex);
    const auto& lines = handle->debug_log_lines;
    if (from_index >= lines.size()) { return {}; }
    return vector<string>(lines.begin() + (ptrdiff_t)from_index, lines.end());
}

} // namespace testing
