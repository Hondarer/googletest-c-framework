#ifdef _WIN32

#include "processController_impl.h"
#include <test_com.h>

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
    int _tl = _getTraceLevel("processController");
    if (_tl > TRACE_NONE) {
        printf("  > startProcessAsync \"%s\"", path.c_str());
        for (const auto& a : args) { printf(" \"%s\"", a.c_str()); }
    }

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
        if (_tl >= TRACE_DETAIL) {
            printf(" -> nullptr\n");
        } else if (_tl > TRACE_NONE) {
            printf("\n");
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

    DWORD creation_flags = CREATE_NEW_PROCESS_GROUP; /* SIGINT 用 */
    if (opts.capture_debug_output) {
        /* このプロセスがデバッガとなり OutputDebugString を受信できる。
         * DEBUG_ONLY_THIS_PROCESS により孫プロセスには伝搬しない。 */
        creation_flags |= DEBUG_ONLY_THIS_PROCESS;
    }

    PROCESS_INFORMATION pi = {};
    BOOL created = CreateProcessA(
        nullptr, cmd_buf.data(), nullptr, nullptr,
        TRUE,            /* bInheritHandles */
        creation_flags,
        env_ptr, nullptr, &si, &pi);

    /* 子プロセス側のハンドルをクローズ */
    CloseHandle(stdin_r);
    CloseHandle(stdout_w);
    CloseHandle(stderr_w);

    if (!created) {
        CloseHandle(stdin_w);
        CloseHandle(stdout_r);
        CloseHandle(stderr_r);
        if (_tl >= TRACE_DETAIL) {
            printf(" -> nullptr\n");
        } else if (_tl > TRACE_NONE) {
            printf("\n");
        }
        return nullptr;
    }
    CloseHandle(pi.hThread); /* スレッドハンドルは不要 */

    auto proc = make_shared<AsyncProcess>();
    proc->proc_handle          = pi.hProcess;
    proc->pid                  = pi.dwProcessId;
    proc->stdin_h              = stdin_w;
    proc->stdout_h             = stdout_r;
    proc->stderr_h             = stderr_r;
    proc->capture_debug_output = opts.capture_debug_output;

    if (_tl >= TRACE_DETAIL) {
        printf(" -> pid=%lu\n", (unsigned long)proc->pid);
    } else if (_tl > TRACE_NONE) {
        printf("\n");
    }

    /* ReaderThread: stdout/stderr/デバッグログを並行読み取り */
    AsyncProcess* p = proc.get();
    proc->reader_thread = thread([p]() {
        unsigned long reader_pid = (unsigned long)p->pid;

        /* stdout 読み取りスレッド */
        thread t_out([p, reader_pid]() {
            char buf[4096];
            DWORD nread = 0;
            string stdout_trace_buf;
            while (ReadFile(p->stdout_h, buf, sizeof(buf), &nread, nullptr) && nread > 0) {
                {
                    lock_guard<mutex> lk(p->buf_mutex);
                    p->stdout_buf.append(buf, nread);
                    p->buf_cv.notify_all();
                }
                /* mutex 解放後にトレース出力 */
                int _tl = _getTraceLevel("processController");
                if (_tl > TRACE_NONE) {
                    stdout_trace_buf.append(buf, nread);
                    size_t pos;
                    while ((pos = stdout_trace_buf.find('\n')) != string::npos) {
                        printf("  > stdout    pid=%lu: \"%s\"\n",
                               reader_pid, stdout_trace_buf.substr(0, pos).c_str());
                        stdout_trace_buf.erase(0, pos + 1);
                    }
                }
            }
        });

        /* stderr 読み取りスレッド */
        thread t_err([p, reader_pid]() {
            char buf[4096];
            DWORD nread = 0;
            string stderr_trace_buf;
            while (ReadFile(p->stderr_h, buf, sizeof(buf), &nread, nullptr) && nread > 0) {
                {
                    lock_guard<mutex> lk(p->buf_mutex);
                    p->stderr_buf.append(buf, nread);
                }
                /* mutex 解放後にトレース出力 */
                int _tl = _getTraceLevel("processController");
                if (_tl > TRACE_NONE) {
                    stderr_trace_buf.append(buf, nread);
                    size_t pos;
                    while ((pos = stderr_trace_buf.find('\n')) != string::npos) {
                        printf("  > stderr    pid=%lu: \"%s\"\n",
                               reader_pid, stderr_trace_buf.substr(0, pos).c_str());
                        stderr_trace_buf.erase(0, pos + 1);
                    }
                }
            }
        });

        /* デバッグイベントループ: OutputDebugString を逐次キャプチャする。
         * capture_debug_output が true の場合のみ起動する。
         * EXIT_PROCESS_DEBUG_EVENT を受信したらループを終了する。 */
        thread t_dbg;
        if (p->capture_debug_output) {
            t_dbg = thread([p, reader_pid]() {
                DEBUG_EVENT de = {};
                while (WaitForDebugEvent(&de, INFINITE)) {
                    DWORD continue_status = DBG_CONTINUE;
                    switch (de.dwDebugEventCode) {

                    case OUTPUT_DEBUG_STRING_EVENT: {
                        const auto& ods = de.u.DebugString;
                        DWORD len = ods.nDebugStringLength;
                        string captured_line;
                        if (len > 0 && len <= 65536) {
                            if (ods.fUnicode == 0) {
                                /* ANSI 文字列 */
                                vector<char> buf(len, '\0');
                                SIZE_T nread = 0;
                                if (ReadProcessMemory(p->proc_handle,
                                        ods.lpDebugStringData,
                                        buf.data(), len, &nread) && nread > 0) {
                                    captured_line = string(buf.data());
                                    lock_guard<mutex> lk(p->buf_mutex);
                                    p->debug_log_lines.push_back(captured_line);
                                }
                            } else {
                                /* Unicode 文字列 (UTF-16LE) → UTF-8 変換 */
                                vector<wchar_t> wbuf(len, L'\0');
                                SIZE_T nread = 0;
                                if (ReadProcessMemory(p->proc_handle,
                                        ods.lpDebugStringData,
                                        wbuf.data(), len * sizeof(wchar_t),
                                        &nread) && nread > 0) {
                                    int u8len = WideCharToMultiByte(
                                        CP_UTF8, 0, wbuf.data(), -1,
                                        nullptr, 0, nullptr, nullptr);
                                    if (u8len > 0) {
                                        vector<char> u8buf(u8len, '\0');
                                        WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), -1,
                                            u8buf.data(), u8len, nullptr, nullptr);
                                        captured_line = string(u8buf.data());
                                        lock_guard<mutex> lk(p->buf_mutex);
                                        p->debug_log_lines.push_back(captured_line);
                                    }
                                }
                            }
                        }
                        /* mutex 解放後にトレース出力 */
                        if (!captured_line.empty()) {
                            int _tl = _getTraceLevel("processController");
                            if (_tl >= TRACE_DETAIL) {
                                printf("  > debug_log pid=%lu: \"%s\"\n", reader_pid, captured_line.c_str());
                            }
                        }
                        break;
                    }

                    case LOAD_DLL_DEBUG_EVENT:
                        /* DLL ロードハンドルをクローズしてリソースリークを防ぐ */
                        if (de.u.LoadDll.hFile != nullptr) {
                            CloseHandle(de.u.LoadDll.hFile);
                        }
                        break;

                    case EXIT_PROCESS_DEBUG_EVENT:
                        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE);
                        return; /* ループ脱出 */

                    case EXCEPTION_DEBUG_EVENT:
                        /* デバッグ開始時の初回ブレークポイント (INT 3) は DBG_CONTINUE で継続。
                         * それ以外の例外はプロセス自身のハンドラに渡す。 */
                        if (de.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT &&
                            de.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_SINGLE_STEP) {
                            continue_status = DBG_EXCEPTION_NOT_HANDLED;
                        }
                        break;

                    default:
                        break;
                    }
                    ContinueDebugEvent(de.dwProcessId, de.dwThreadId, continue_status);
                }
            });
        }

        t_out.join();
        t_err.join();
        if (t_dbg.joinable()) { t_dbg.join(); }

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
    if (_getTraceLevel("processController") > TRACE_NONE) {
        printf("  > interruptProcess pid=%lu\n", (unsigned long)handle->pid);
    }
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, handle->pid);
}

/* -------- killProcess -------- */

void killProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->proc_handle == nullptr) { return; }
    if (_getTraceLevel("processController") > TRACE_NONE) {
        printf("  > killProcess pid=%lu\n", (unsigned long)handle->pid);
    }
    TerminateProcess(handle->proc_handle, (UINT)-1);
}

/* -------- writeStdinImpl -------- */

bool writeStdinImpl(AsyncProcessHandle& handle, const string& data)
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
    if (_getTraceLevel("processController") > TRACE_NONE) {
        printf("  > closeStdin pid=%lu\n", (unsigned long)handle->pid);
    }
    CloseHandle(handle->stdin_h);
    handle->stdin_h = nullptr;
}

/* -------- waitProcess -------- */

int waitProcess(AsyncProcessHandle& handle, int timeout_ms)
{
    if (!handle) { return -1; }

    int _tl = _getTraceLevel("processController");
    unsigned long trace_pid = (unsigned long)handle->pid;

    /* 二重呼び出し時は cached 終了コードを返す */
    if (handle->proc_handle == nullptr && !handle->reader_thread.joinable()) {
        if (_tl > TRACE_NONE) {
            printf("  > waitProcess pid=%lu timeout=%dms", trace_pid, timeout_ms);
            if (_tl >= TRACE_DETAIL) {
                printf(" -> exit_code=%d\n", handle->last_exit_code);
            } else {
                printf("\n");
            }
        }
        return handle->last_exit_code;
    }

    if (_tl > TRACE_NONE) {
        printf("  > waitProcess pid=%lu timeout=%dms\n", trace_pid, timeout_ms);
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
        handle->pid = 0;
    }

    if (handle->reader_thread.joinable()) {
        handle->reader_thread.join();
    }

    /* ハンドルをクローズ (reader_thread join 後に実施)。
     * proc_handle は t_dbg スレッドが ReadProcessMemory で使用するため join 後に閉じる。 */
    if (handle->proc_handle != nullptr) { CloseHandle(handle->proc_handle); handle->proc_handle = nullptr; }
    if (handle->stdout_h    != nullptr) { CloseHandle(handle->stdout_h);    handle->stdout_h    = nullptr; }
    if (handle->stderr_h    != nullptr) { CloseHandle(handle->stderr_h);    handle->stderr_h    = nullptr; }

    handle->last_exit_code = exit_code;

    if (_tl > TRACE_NONE) {
        printf("  > waitProcess pid=%lu exit_code=%d\n", trace_pid, exit_code);
    }
    return exit_code;
}

} // namespace testing

#endif /* _WIN32 */
