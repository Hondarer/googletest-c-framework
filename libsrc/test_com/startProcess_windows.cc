#ifdef _WIN32

#include "startProcess_impl.h"

#include <chrono>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace testing {

/* -------- ユーティリティ -------- */

/** 現在の環境変数に overrides を上書きした環境変数ブロック (NUL 区切り) を返す。 */
static string buildEnvBlock(const map<string, string>& overrides)
{
    LPCH cur = GetEnvironmentStrings();
    if (cur == nullptr) { return ""; }

    vector<string> entries;
    for (LPCH p = cur; *p != '\0'; ) {
        string s(p);
        entries.push_back(s);
        p += s.size() + 1;
    }
    FreeEnvironmentStrings(cur);

    for (const auto& kv : overrides) {
        bool found = false;
        for (auto& e : entries) {
            size_t eq = e.find('=');
            if (eq == string::npos || eq == 0) { continue; }
            string key = e.substr(0, eq);
            if (_stricmp(key.c_str(), kv.first.c_str()) == 0) {
                e = kv.first + "=" + kv.second;
                found = true;
                break;
            }
        }
        if (!found) { entries.push_back(kv.first + "=" + kv.second); }
    }

    string block;
    for (const auto& e : entries) { block += e; block += '\0'; }
    block += '\0';
    return block;
}

/* -------- AsyncProcess デストラクタ -------- */

AsyncProcess::~AsyncProcess()
{
    if (proc_handle != nullptr) {
        TerminateProcess(proc_handle, (UINT)-1);
        WaitForSingleObject(proc_handle, 5000);
    }
    if (reader_thread.joinable()) {
        reader_thread.join();
    }
    if (proc_handle != nullptr) { CloseHandle(proc_handle); proc_handle = nullptr; }
    if (stdin_h     != nullptr) { CloseHandle(stdin_h);     stdin_h     = nullptr; }
    if (stdout_h    != nullptr) { CloseHandle(stdout_h);    stdout_h    = nullptr; }
    if (stderr_h    != nullptr) { CloseHandle(stderr_h);    stderr_h    = nullptr; }
}

/* -------- startProcessAsync -------- */

AsyncProcessHandle startProcessAsync(const string& path,
                                      const vector<string>& args,
                                      const ProcessOptions& opts)
{
    SECURITY_ATTRIBUTES sa = {};
    sa.nLength        = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE stdin_r  = nullptr, stdin_w  = nullptr;
    HANDLE stdout_r = nullptr, stdout_w = nullptr;
    HANDLE stderr_r = nullptr, stderr_w = nullptr;

    if (!CreatePipe(&stdin_r,  &stdin_w,  &sa, 0) ||
        !CreatePipe(&stdout_r, &stdout_w, &sa, 0) ||
        !CreatePipe(&stderr_r, &stderr_w, &sa, 0)) {
        for (HANDLE h : {stdin_r, stdin_w, stdout_r, stdout_w, stderr_r, stderr_w}) {
            if (h != nullptr) { CloseHandle(h); }
        }
        return nullptr;
    }

    /* 親プロセス側のハンドルを継承不可にする */
    SetHandleInformation(stdin_w,  HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stdout_r, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stderr_r, HANDLE_FLAG_INHERIT, 0);

    /* コマンドライン文字列 */
    string cmd = "\"" + path + "\"";
    for (const auto& a : args) { cmd += " \"" + a + "\""; }
    vector<char> cmd_buf(cmd.begin(), cmd.end());
    cmd_buf.push_back('\0');

    STARTUPINFOA si = {};
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESTDHANDLES;
    si.hStdInput   = stdin_r;
    si.hStdOutput  = stdout_w;
    si.hStdError   = stderr_w;

    string env_block;
    LPVOID env_ptr = nullptr;
    if (!opts.env_set.empty()) {
        env_block = buildEnvBlock(opts.env_set);
        env_ptr   = (LPVOID)env_block.data();
    }

    PROCESS_INFORMATION pi = {};
    BOOL created = CreateProcessA(
        nullptr, cmd_buf.data(), nullptr, nullptr,
        TRUE,                       /* bInheritHandles */
        CREATE_NEW_PROCESS_GROUP,   /* dwCreationFlags (SIGINT 用) */
        env_ptr, nullptr, &si, &pi);

    /* 子プロセス側のハンドルをクローズ */
    CloseHandle(stdin_r);
    CloseHandle(stdout_w);
    CloseHandle(stderr_w);

    if (!created) {
        CloseHandle(stdin_w);
        CloseHandle(stdout_r);
        CloseHandle(stderr_r);
        return nullptr;
    }
    CloseHandle(pi.hThread); /* スレッドハンドルは不要 */

    auto proc = make_shared<AsyncProcess>();
    proc->proc_handle = pi.hProcess;
    proc->pid         = pi.dwProcessId;
    proc->stdin_h     = stdin_w;
    proc->stdout_h    = stdout_r;
    proc->stderr_h    = stderr_r;

    /* ReaderThread: stdout/stderr を別スレッドで並行読み取り */
    AsyncProcess* p = proc.get();
    proc->reader_thread = thread([p]() {
        /* stdout 読み取りスレッド */
        thread t_out([p]() {
            char buf[4096];
            DWORD nread = 0;
            while (ReadFile(p->stdout_h, buf, sizeof(buf), &nread, nullptr) && nread > 0) {
                lock_guard<mutex> lk(p->buf_mutex);
                p->stdout_buf.append(buf, nread);
                p->buf_cv.notify_all();
            }
        });

        /* stderr 読み取りスレッド */
        thread t_err([p]() {
            char buf[4096];
            DWORD nread = 0;
            while (ReadFile(p->stderr_h, buf, sizeof(buf), &nread, nullptr) && nread > 0) {
                lock_guard<mutex> lk(p->buf_mutex);
                p->stderr_buf.append(buf, nread);
            }
        });

        t_out.join();
        t_err.join();

        lock_guard<mutex> lk(p->buf_mutex);
        p->process_done = true;
        p->buf_cv.notify_all();
    });

    return proc;
}

/* -------- interruptProcess -------- */

void interruptProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->proc_handle == nullptr) { return; }
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, handle->pid);
}

/* -------- killProcess -------- */

void killProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->proc_handle == nullptr) { return; }
    TerminateProcess(handle->proc_handle, (UINT)-1);
}

/* -------- writeStdin -------- */

bool writeStdin(AsyncProcessHandle& handle, const string& data)
{
    if (!handle || handle->stdin_h == nullptr) { return false; }
    DWORD written = 0;
    BOOL ok = WriteFile(handle->stdin_h, data.data(), (DWORD)data.size(), &written, nullptr);
    return ok && (written == (DWORD)data.size());
}

/* -------- closeStdin -------- */

void closeStdin(AsyncProcessHandle& handle)
{
    if (!handle || handle->stdin_h == nullptr) { return; }
    CloseHandle(handle->stdin_h);
    handle->stdin_h = nullptr;
}

/* -------- waitProcess -------- */

int waitProcess(AsyncProcessHandle& handle, int timeout_ms)
{
    if (!handle) { return -1; }

    /* 二重呼び出し時は cached 終了コードを返す */
    if (handle->proc_handle == nullptr && !handle->reader_thread.joinable()) {
        return handle->last_exit_code;
    }

    /* stdin を閉じて子プロセスに EOF を通知 */
    if (handle->stdin_h != nullptr) {
        CloseHandle(handle->stdin_h);
        handle->stdin_h = nullptr;
    }

    int exit_code = -1;

    if (handle->proc_handle != nullptr) {
        DWORD ms = (timeout_ms < 0) ? INFINITE : (DWORD)timeout_ms;
        DWORD ret = WaitForSingleObject(handle->proc_handle, ms);
        if (ret == WAIT_TIMEOUT) {
            TerminateProcess(handle->proc_handle, (UINT)-1);
            WaitForSingleObject(handle->proc_handle, 5000);
            exit_code = -1;
        } else {
            DWORD ec = (DWORD)-1;
            GetExitCodeProcess(handle->proc_handle, &ec);
            exit_code = (int)ec;
        }
        CloseHandle(handle->proc_handle);
        handle->proc_handle = nullptr;
        handle->pid = 0;
    }

    if (handle->reader_thread.joinable()) {
        handle->reader_thread.join();
    }

    /* ハンドルをクローズ (reader_thread join 後に実施) */
    if (handle->stdout_h != nullptr) { CloseHandle(handle->stdout_h); handle->stdout_h = nullptr; }
    if (handle->stderr_h != nullptr) { CloseHandle(handle->stderr_h); handle->stderr_h = nullptr; }

    handle->last_exit_code = exit_code;
    return exit_code;
}

} // namespace testing

#endif /* _WIN32 */
