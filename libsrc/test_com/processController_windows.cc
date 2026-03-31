#ifdef _WIN32

#include "processController_impl.h"
#include <test_com.h>

#include <chrono>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <evntrace.h>
#include <evntcons.h>
#pragma comment(lib, "Advapi32.lib")

#ifndef INVALID_PROCESSTRACE_HANDLE
#define INVALID_PROCESSTRACE_HANDLE ((TRACEHANDLE)INVALID_HANDLE_VALUE)
#endif

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

/* -------- ETW キャプチャ ヘルパー -------- */

/** GUID 文字列 "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" をパースする。成功 0 / 失敗 -1。 */
static int parseGuid(const char *str, GUID *guid)
{
    unsigned int d[11];
    if (str == nullptr || guid == nullptr) { return -1; }
    int n = sscanf_s(str,
        "%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x",
        &d[0], &d[1], &d[2], &d[3], &d[4],
        &d[5], &d[6], &d[7], &d[8], &d[9], &d[10]);
    if (n != 11) { return -1; }
    guid->Data1    = (ULONG)d[0];
    guid->Data2    = (USHORT)d[1];
    guid->Data3    = (USHORT)d[2];
    guid->Data4[0] = (UCHAR)d[3];
    guid->Data4[1] = (UCHAR)d[4];
    guid->Data4[2] = (UCHAR)d[5];
    guid->Data4[3] = (UCHAR)d[6];
    guid->Data4[4] = (UCHAR)d[7];
    guid->Data4[5] = (UCHAR)d[8];
    guid->Data4[6] = (UCHAR)d[9];
    guid->Data4[7] = (UCHAR)d[10];
    return 0;
}

/** GUID 一致判定。 */
static bool guidEqual(const GUID *a, const GUID *b)
{
    return a->Data1 == b->Data1 &&
           a->Data2 == b->Data2 &&
           a->Data3 == b->Data3 &&
           a->Data4[0] == b->Data4[0] &&
           a->Data4[1] == b->Data4[1] &&
           a->Data4[2] == b->Data4[2] &&
           a->Data4[3] == b->Data4[3] &&
           a->Data4[4] == b->Data4[4] &&
           a->Data4[5] == b->Data4[5] &&
           a->Data4[6] == b->Data4[6] &&
           a->Data4[7] == b->Data4[7];
}

/** ETW コールバックに渡すコンテキスト。 */
struct EtwCallbackData {
    AsyncProcess* proc;
    DWORD         target_pid;
    GUID          provider_guid;
    string        service_filter;
};

/** UserData から Service と Message を抽出する。
 *  TraceLogging 形式:
 *    2フィールド (Service あり): "Service\0Message\0"
 *    1フィールド (Service なし): "Message\0"
 *  戻り値: message (空文字列の場合はイベントにデータなし)。 */
static void parseUserData(const void *userData, USHORT userDataLength,
                          string &out_service, string &out_message)
{
    out_service.clear();
    out_message.clear();

    if (userData == nullptr || userDataLength == 0) { return; }

    const char *data = static_cast<const char *>(userData);
    const char *end  = data + userDataLength;

    /* null 終端文字列を順番に抽出 */
    vector<string> fields;
    while (data < end) {
        string field(data);
        data += field.size() + 1;
        fields.push_back(std::move(field));
    }

    if (fields.size() >= 2) {
        out_service = fields[0];
        out_message = fields[1];
    } else if (fields.size() == 1) {
        out_message = fields[0];
    }
}

/** ETW イベントレコードコールバック (ProcessTrace から呼ばれる)。 */
static VOID WINAPI etwEventCallback(PEVENT_RECORD pEvent)
{
    if (pEvent == nullptr) { return; }

    auto *ctx = static_cast<EtwCallbackData *>(pEvent->UserContext);
    if (ctx == nullptr || ctx->proc == nullptr) { return; }

    /* プロバイダ GUID フィルタ */
    if (!guidEqual(&pEvent->EventHeader.ProviderId, &ctx->provider_guid)) { return; }

    /* PID フィルタ */
    if (pEvent->EventHeader.ProcessId != ctx->target_pid) { return; }

    /* UserData パース */
    string service, message;
    parseUserData(pEvent->UserData, pEvent->UserDataLength, service, message);

    /* Service フィルタ */
    if (!ctx->service_filter.empty() && service != ctx->service_filter) { return; }

    if (message.empty()) { return; }

    /* debug_log_lines に追加 */
    {
        lock_guard<mutex> lk(ctx->proc->buf_mutex);
        ctx->proc->debug_log_lines.push_back(message);
    }

    int _tl = _getTraceLevel("processController");
    if (_tl >= TRACE_DETAIL) {
        printf("  > etw_log   pid=%lu: \"%s\"\n",
               (unsigned long)ctx->target_pid, message.c_str());
    }
}

/** ProcessTrace ワーカースレッド関数。 */
static DWORD WINAPI etwTraceThreadProc(LPVOID param)
{
    TRACEHANDLE *pTraceHandle = static_cast<TRACEHANDLE *>(param);
    ProcessTrace(pTraceHandle, 1, NULL, NULL);
    return 0;
}

/** ETW セッション管理構造体 (スレッドローカル)。 */
struct EtwSession {
    TRACEHANDLE              session_handle = 0;
    TRACEHANDLE              trace_handle   = INVALID_PROCESSTRACE_HANDLE;
    HANDLE                   thread_handle  = nullptr;
    EVENT_TRACE_PROPERTIES  *properties     = nullptr;
    wchar_t                 *session_name_w = nullptr;
    EtwCallbackData          callback_data  = {};

    ~EtwSession() { stop(); }

    /** セッション開始。成功時 true。 */
    bool start(AsyncProcess *proc, DWORD target_pid,
               const string &provider_guid_str, const string &service_filter)
    {
        GUID guid;
        if (parseGuid(provider_guid_str.c_str(), &guid) != 0) { return false; }

        callback_data.proc           = proc;
        callback_data.target_pid     = target_pid;
        callback_data.provider_guid  = guid;
        callback_data.service_filter = service_filter;

        /* セッション名: "testfw_<テストPID>_<子PID>" */
        char name_buf[64];
        snprintf(name_buf, sizeof(name_buf), "testfw_%lu_%lu",
                 (unsigned long)GetCurrentProcessId(), (unsigned long)target_pid);

        int name_len_w = MultiByteToWideChar(CP_UTF8, 0, name_buf, -1, NULL, 0);
        if (name_len_w <= 0) { return false; }

        session_name_w = static_cast<wchar_t *>(malloc((size_t)name_len_w * sizeof(wchar_t)));
        if (session_name_w == nullptr) { return false; }
        MultiByteToWideChar(CP_UTF8, 0, name_buf, -1, session_name_w, name_len_w);

        size_t props_size = sizeof(EVENT_TRACE_PROPERTIES) + ((size_t)name_len_w * sizeof(wchar_t));
        properties = static_cast<EVENT_TRACE_PROPERTIES *>(malloc(props_size));
        if (properties == nullptr) { stop(); return false; }

        ZeroMemory(properties, props_size);
        properties->Wnode.BufferSize    = (ULONG)props_size;
        properties->Wnode.Flags         = WNODE_FLAG_TRACED_GUID;
        properties->Wnode.ClientContext = 1; /* QPC */
        properties->LogFileMode         = EVENT_TRACE_REAL_TIME_MODE;
        properties->LoggerNameOffset    = sizeof(EVENT_TRACE_PROPERTIES);
        properties->FlushTimer          = 1;

        ULONG status = StartTraceW(&session_handle, session_name_w, properties);
        if (status != ERROR_SUCCESS) {
            session_handle = 0;
            stop();
            return false;
        }

        /* プロバイダを有効化 */
        ENABLE_TRACE_PARAMETERS etp = {};
        etp.Version = ENABLE_TRACE_PARAMETERS_VERSION_2;
        status = EnableTraceEx2(session_handle, &guid,
                                EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                                TRACE_LEVEL_VERBOSE,
                                0xFFFFFFFFFFFFFFFF, 0, 0, &etp);
        if (status != ERROR_SUCCESS) { stop(); return false; }

        /* トレースをオープン */
        EVENT_TRACE_LOGFILEW trace_logfile = {};
        trace_logfile.LoggerName          = session_name_w;
        trace_logfile.ProcessTraceMode    = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
        trace_logfile.EventRecordCallback = etwEventCallback;
        trace_logfile.Context             = &callback_data;

        trace_handle = OpenTraceW(&trace_logfile);
        if (trace_handle == INVALID_PROCESSTRACE_HANDLE) { stop(); return false; }

        /* ProcessTrace ワーカースレッドを起動 */
        thread_handle = CreateThread(NULL, 0, etwTraceThreadProc, &trace_handle, 0, NULL);
        if (thread_handle == nullptr) { stop(); return false; }

        return true;
    }

    /** セッション停止とリソース解放。 */
    void stop()
    {
        if (session_handle != 0 && properties != nullptr) {
            ControlTraceW(session_handle, NULL, properties, EVENT_TRACE_CONTROL_STOP);
            session_handle = 0;
        }
        if (thread_handle != nullptr) {
            WaitForSingleObject(thread_handle, INFINITE);
            CloseHandle(thread_handle);
            thread_handle = nullptr;
        }
        if (trace_handle != INVALID_PROCESSTRACE_HANDLE) {
            CloseTrace(trace_handle);
            trace_handle = INVALID_PROCESSTRACE_HANDLE;
        }
        free(session_name_w);  session_name_w = nullptr;
        free(properties);      properties     = nullptr;
    }

    /* コピー禁止 */
    EtwSession(const EtwSession&)            = delete;
    EtwSession& operator=(const EtwSession&) = delete;
    EtwSession() = default;
};

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
    proc->etw_provider_guid    = opts.etw_provider_guid;
    proc->etw_service_filter   = opts.etw_service_filter;

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

        /* dbwin 方式で OutputDebugString をキャプチャするスレッド。
         * WaitForDebugEvent は CreateProcess を呼び出したスレッドからのみ呼び出し可能であり、
         * 別スレッドから呼び出すとプロセスがデバッグ初期状態で永久に凍結する。
         * そのため DEBUG_ONLY_THIS_PROCESS を使用せず、共有メモリ経由でキャプチャする。
         * capture_debug_output が true の場合のみ起動する。 */
        thread t_dbg;
        if (p->capture_debug_output) {
            t_dbg = thread([p, reader_pid]() {
                HANDLE hBufReady = CreateEventA(nullptr, FALSE, FALSE, "DBWIN_BUFFER_READY");
                HANDLE hDataReady = CreateEventA(nullptr, FALSE, FALSE, "DBWIN_DATA_READY");
                HANDLE hMapping   = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr,
                                        PAGE_READWRITE, 0, 4096, "DBWIN_BUFFER");
                if (!hBufReady || !hDataReady || !hMapping) {
                    if (hBufReady)  { CloseHandle(hBufReady);  }
                    if (hDataReady) { CloseHandle(hDataReady); }
                    if (hMapping)   { CloseHandle(hMapping);   }
                    return;
                }
                struct DbwinView { DWORD pid; char msg[4096 - sizeof(DWORD)]; };
                DbwinView* view = static_cast<DbwinView*>(
                    MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
                if (!view) {
                    CloseHandle(hBufReady);
                    CloseHandle(hDataReady);
                    CloseHandle(hMapping);
                    return;
                }

                HANDLE proc_h = p->proc_handle; /* waitForExit が閉じるまで有効 */

                SetEvent(hBufReady); /* バッファ受信準備完了を通知 */

                for (;;) {
                    DWORD dr = WaitForSingleObject(hDataReady, 200);
                    if (dr == WAIT_OBJECT_0) {
                        if (view->pid == (DWORD)reader_pid) {
                            string captured_line(view->msg);
                            {
                                lock_guard<mutex> lk(p->buf_mutex);
                                p->debug_log_lines.push_back(captured_line);
                            }
                            int _tl = _getTraceLevel("processController");
                            if (_tl >= TRACE_DETAIL) {
                                printf("  > debug_log pid=%lu: \"%s\"\n",
                                       reader_pid, captured_line.c_str());
                            }
                        }
                        SetEvent(hBufReady); /* 次のメッセージを受信可能にする */
                    } else {
                        /* タイムアウト: プロセス終了チェック */
                        if (proc_h == nullptr ||
                            WaitForSingleObject(proc_h, 0) == WAIT_OBJECT_0) {
                            break;
                        }
                    }
                }

                UnmapViewOfFile(view);
                CloseHandle(hMapping);
                CloseHandle(hDataReady);
                CloseHandle(hBufReady);
            });
        }

        /* ETW イベントをキャプチャするスレッド。
         * etw_provider_guid が非空の場合のみ起動する。
         * ProcessTrace はブロッキング呼び出しのため専用スレッドで実行し、
         * プロセス終了後にセッションを停止して ProcessTrace を終了させる。 */
        EtwSession etw_session;
        if (!p->etw_provider_guid.empty()) {
            bool ok = etw_session.start(p, (DWORD)reader_pid,
                                        p->etw_provider_guid, p->etw_service_filter);
            int _tl = _getTraceLevel("processController");
            if (_tl >= TRACE_DETAIL) {
                printf("  > etw_session pid=%lu: %s\n",
                       reader_pid, ok ? "started" : "skipped (start failed)");
            }
        }

        t_out.join();
        t_err.join();
        if (t_dbg.joinable()) { t_dbg.join(); }

        /* ETW セッション停止 (ProcessTrace 終了 → スレッド join) */
        etw_session.stop();

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

/* -------- waitForExit -------- */

int waitForExit(AsyncProcessHandle& handle, int timeout_ms)
{
    if (!handle) { return -1; }

    int _tl = _getTraceLevel("processController");
    unsigned long trace_pid = (unsigned long)handle->pid;

    /* 二重呼び出し時は cached 終了コードを返す */
    if (handle->proc_handle == nullptr && !handle->reader_thread.joinable()) {
        if (_tl > TRACE_NONE) {
            printf("  > waitForExit pid=%lu timeout=%dms", trace_pid, timeout_ms);
            if (_tl >= TRACE_DETAIL) {
                printf(" -> exit_code=%d\n", handle->last_exit_code);
            } else {
                printf("\n");
            }
        }
        return handle->last_exit_code;
    }

    if (_tl > TRACE_NONE) {
        printf("  > waitForExit pid=%lu timeout=%dms\n", trace_pid, timeout_ms);
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
        printf("  > waitForExit pid=%lu exit_code=%d\n", trace_pid, exit_code);
    }
    return exit_code;
}

} // namespace testing

#endif /* _WIN32 */
