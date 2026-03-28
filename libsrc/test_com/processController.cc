/* プラットフォーム共通の非同期プロセス制御 API 実装。
 * プラットフォーム固有の関数 (startProcessAsync / writeStdinImpl / closeStdin / waitProcess /
 * interruptProcess / killProcess) は processController_linux.cc / processController_windows.cc に実装。 */

#include "processController_impl.h"
#include <test_com.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

namespace testing {

/* -------- writeStdin -------- */

bool writeStdin(AsyncProcessHandle& handle, const string& data)
{
    if (_getTraceLevel("processController") > TRACE_NONE) {
        int pid = handle ? (int)handle->pid : -1;
        printf("  > writeStdin pid=%d \"%s\"\n", pid, data.c_str());
    }
    return writeStdinImpl(handle, data);
}

/* -------- writeLineStdin -------- */

bool writeLineStdin(AsyncProcessHandle& handle, const string& line)
{
    if (_getTraceLevel("processController") > TRACE_NONE) {
        int pid = handle ? (int)handle->pid : -1;
        printf("  > writeLineStdin pid=%d \"%s\"\n", pid, line.c_str());
    }
    return writeStdinImpl(handle, line + "\n");
}

/* -------- waitForOutput -------- */

string waitForOutput(AsyncProcessHandle& handle,
                     const string& pattern,
                     int timeout_ms)
{
    if (!handle) {
        throw runtime_error("waitForOutput: null handle");
    }

    int _tl = _getTraceLevel("processController");
    int pid = (int)handle->pid;
    if (_tl > TRACE_NONE) {
        printf("  > waitForOutput pid=%d \"%s\" timeout=%dms\n", pid, pattern.c_str(), timeout_ms);
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
        if (_tl > TRACE_NONE) {
            printf("  > waitForOutput pid=%d \"%s\" timeout\n", pid, pattern.c_str());
        }
        throw runtime_error(
            "waitForOutput: timeout or EOF before pattern: \"" + pattern + "\"");
    }

    if (_tl > TRACE_NONE) {
        printf("  > waitForOutput pid=%d \"%s\" matched\n", pid, pattern.c_str());
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
