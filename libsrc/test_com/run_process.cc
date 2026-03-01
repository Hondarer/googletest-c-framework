#include <run_process.h>

#include <array>
#include <thread>

#ifndef _WIN32
/* =========================================================
 *  Linux 実装
 *  pipe + fork + execv + std::thread で
 *  stdin / stdout / stderr / syslog をキャプチャする。
 * ========================================================= */

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

/** ファイルディスクリプタからすべてのデータを読み込む */
static string readAllFd(int fd)
{
    string result;
    array<char, 4096> buf;
    ssize_t n;
    while ((n = read(fd, buf.data(), buf.size())) > 0)
    {
        result.append(buf.data(), (size_t)n);
    }
    return result;
}

/** ファイルの内容をすべて読み込む */
static string readFileAll(const string &path)
{
    string result;
    FILE *f = fopen(path.c_str(), "r");
    if (f == nullptr)
    {
        return result;
    }
    array<char, 4096> buf;
    while (fgets(buf.data(), (int)buf.size(), f) != nullptr)
    {
        result += buf.data();
    }
    fclose(f);
    return result;
}

testing::ProcessResult testing::runProcess(const string &binary, const ProcessOptions &opts)
{
    ProcessResult result;
    result.exit_code = -1;

    /* preload_lib が指定されているがファイルが存在しない場合は即座に失敗する。
     * ld.so は存在しないライブラリを LD_PRELOAD で指定されても警告のみで続行するため、
     * syslog がインターセプトされずに debug_log が空になるという無言の失敗を防ぐ。 */
    if (!opts.preload_lib.empty() && access(opts.preload_lib.c_str(), F_OK) != 0)
    {
        fprintf(stderr, "runProcess: preload_lib not found: %s\n", opts.preload_lib.c_str());
        return result;
    }

    /* パイプ作成: [0]=読み込み端, [1]=書き込み端 */
    int stdin_pipe[2] = {-1, -1};
    int stdout_pipe[2] = {-1, -1};
    int stderr_pipe[2] = {-1, -1};

    if (pipe(stdin_pipe) != 0 || pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0)
    {
        for (int fd : {stdin_pipe[0], stdin_pipe[1], stdout_pipe[0], stdout_pipe[1], stderr_pipe[0], stderr_pipe[1]})
        {
            if (fd != -1)
            {
                close(fd);
            }
        }
        return result;
    }

    /* syslog モック用一時ファイルの作成 */
    string syslog_tmp;
    if (!opts.preload_lib.empty())
    {
        char tmp[] = "/tmp/syslog_XXXXXX";
        int fd = mkstemp(tmp);
        if (fd == -1)
        {
            for (int pipe_fd : {stdin_pipe[0], stdin_pipe[1], stdout_pipe[0], stdout_pipe[1], stderr_pipe[0], stderr_pipe[1]})
            {
                close(pipe_fd);
            }
            return result;
        }
        close(fd);
        syslog_tmp = tmp;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        for (int fd : {stdin_pipe[0], stdin_pipe[1], stdout_pipe[0], stdout_pipe[1], stderr_pipe[0], stderr_pipe[1]})
        {
            close(fd);
        }
        if (!syslog_tmp.empty())
        {
            unlink(syslog_tmp.c_str());
        }
        return result;
    }

    if (pid == 0)
    {
        /* ──────────────────────────────────────────────────────
         * 子プロセス
         * ────────────────────────────────────────────────────── */

        /* 親プロセス側のパイプ端をクローズ */
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        /* 標準入出力をリダイレクト */
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        /* 環境変数の設定 */
        for (const auto &kv : opts.env_set)
        {
            setenv(kv.first.c_str(), kv.second.c_str(), 1);
        }

        if (!opts.preload_lib.empty())
        {
            /* syslog モックが出力先ファイルパスを読む環境変数 */
            setenv("SYSLOG_MOCK_FILE", syslog_tmp.c_str(), 1);

            /* 既存の LD_PRELOAD があれば保持して追記する */
            const char *existing = getenv("LD_PRELOAD");
            string preload_val = opts.preload_lib;
            if (existing != nullptr && existing[0] != '\0')
            {
                preload_val += ":" + string(existing);
            }
            setenv("LD_PRELOAD", preload_val.c_str(), 1);
        }

        /* argv の構築。
         * execv() は引数文字列を変更しない (POSIX 保証) ため const_cast は安全。
         * execv() のシグネチャが char *const * を要求するので vector<char *> を使う。 */
        vector<char *> argv;
        argv.push_back(const_cast<char *>(binary.c_str()));
        for (const auto &a : opts.args)
        {
            argv.push_back(const_cast<char *>(a.c_str()));
        }
        argv.push_back(nullptr);

        execv(binary.c_str(), argv.data());
        _exit(127); /* execv 失敗時のみ到達 */
    }

    /* ──────────────────────────────────────────────────────
     * 親プロセス
     * ────────────────────────────────────────────────────── */

    /* 子プロセス側のパイプ端をクローズ */
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    /* stdin に渡すデータを構築 (各行に \n を付けて結合) */
    string stdin_data;
    for (const auto &line : opts.stdin_lines)
    {
        stdin_data += line + "\n";
    }

    /* stdin 書き込み / stdout 読み込み / stderr 読み込みを並行実行。
     * 直列実行するとパイプバッファが満杯でデッドロックが発生するため
     * std::thread で分離する。 */
    string stdout_out, stderr_out;

    thread stdin_thread([&]() {
        /* SIGPIPE をブロックして write() 失敗を errno で検出できるようにする。
         * 子プロセスが stdin を早期にクローズした場合の保護。 */
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGPIPE);
        pthread_sigmask(SIG_BLOCK, &mask, nullptr);

        if (!stdin_data.empty())
        {
            const char *p = stdin_data.data();
            size_t remain = stdin_data.size();
            while (remain > 0)
            {
                ssize_t n = write(stdin_pipe[1], p, remain);
                if (n <= 0)
                {
                    break;
                }
                p += n;
                remain -= (size_t)n;
            }
        }
        close(stdin_pipe[1]);
    });

    thread stdout_thread([&]() {
        stdout_out = readAllFd(stdout_pipe[0]);
        close(stdout_pipe[0]);
    });

    thread stderr_thread([&]() {
        stderr_out = readAllFd(stderr_pipe[0]);
        close(stderr_pipe[0]);
    });

    stdin_thread.join();
    stdout_thread.join();
    stderr_thread.join();

    int status = 0;
    waitpid(pid, &status, 0);

    result.exit_code = WEXITSTATUS(status);
    result.stdout_out = stdout_out;
    result.stderr_out = stderr_out;

    if (!syslog_tmp.empty())
    {
        result.debug_log = readFileAll(syslog_tmp);
        unlink(syslog_tmp.c_str());
    }

    return result;
}

#else /* _WIN32 */
/* =========================================================
 *  Windows 実装
 *  CreateProcess(DEBUG_PROCESS) + CreatePipe + std::thread で
 *  stdin / stdout / stderr / OutputDebugString をキャプチャする。
 * ========================================================= */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef byte
    #undef byte /* std::byte と windows.h の byte 型の競合を解消する */
#endif

#include <vector>

/** 現在の環境変数に overrides を上書きした環境変数ブロックを構築する。
 *  返値は CreateProcessA の lpEnvironment に渡せる ANSI 形式のブロック。
 *  ブロック末尾は "KEY=VALUE\0...\0\0" の二重 NUL で終端される。 */
static string buildEnvBlock(const map<string, string> &overrides)
{
    LPCH cur = GetEnvironmentStrings();
    if (cur == nullptr)
    {
        return "";
    }

    /* 現在の環境変数をエントリリストに変換 */
    vector<string> entries;
    for (LPCH p = cur; *p != '\0';)
    {
        string s(p);
        entries.push_back(s);
        p += s.size() + 1;
    }
    FreeEnvironmentStrings(cur);

    /* overrides を適用 (キーは大文字小文字を区別しない) */
    for (const auto &kv : overrides)
    {
        bool found = false;
        for (auto &e : entries)
        {
            size_t eq = e.find('=');
            if (eq == string::npos || eq == 0)
            {
                continue;
            }
            string key = e.substr(0, eq);
            if (_stricmp(key.c_str(), kv.first.c_str()) == 0)
            {
                e = kv.first + "=" + kv.second;
                found = true;
                break;
            }
        }
        if (!found)
        {
            entries.push_back(kv.first + "=" + kv.second);
        }
    }

    /* ブロックをシリアライズ: KEY=VALUE\0 ... \0 */
    string block;
    for (const auto &e : entries)
    {
        block += e;
        block += '\0';
    }
    block += '\0'; /* 終端の二重 NUL */
    return block;
}

/** ReadFile ループで HANDLE からすべてのデータを読み込む */
static string readAllHandle(HANDLE h)
{
    string result;
    array<char, 4096> buf;
    DWORD nread = 0;
    while (ReadFile(h, buf.data(), (DWORD)buf.size(), &nread, nullptr) && nread > 0)
    {
        result.append(buf.data(), nread);
    }
    return result;
}

testing::ProcessResult testing::runProcess(const string &binary, const ProcessOptions &opts)
{
    ProcessResult result;
    result.exit_code = -1;

    /* 子プロセスが継承できる SECURITY_ATTRIBUTES */
    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    /* stdin / stdout / stderr パイプを作成 */
    HANDLE stdin_r = nullptr, stdin_w = nullptr;
    HANDLE stdout_r = nullptr, stdout_w = nullptr;
    HANDLE stderr_r = nullptr, stderr_w = nullptr;

    if (!CreatePipe(&stdin_r, &stdin_w, &sa, 0) ||
        !CreatePipe(&stdout_r, &stdout_w, &sa, 0) ||
        !CreatePipe(&stderr_r, &stderr_w, &sa, 0))
    {
        for (HANDLE h : {stdin_r, stdin_w, stdout_r, stdout_w, stderr_r, stderr_w})
        {
            if (h != nullptr)
            {
                CloseHandle(h);
            }
        }
        return result;
    }

    /* 親プロセス側のハンドルを継承不可にする */
    SetHandleInformation(stdin_w, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stdout_r, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stderr_r, HANDLE_FLAG_INHERIT, 0);

    /* コマンドライン文字列を構築 */
    string cmd = "\"" + binary + "\"";
    for (const auto &a : opts.args)
    {
        cmd += " \"" + a + "\"";
    }
    vector<char> cmd_buf(cmd.begin(), cmd.end());
    cmd_buf.push_back('\0');

    /* STARTUPINFO: 標準ハンドルをパイプにリダイレクト */
    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = stdin_r;
    si.hStdOutput = stdout_w;
    si.hStdError = stderr_w;

    /* 環境変数ブロック */
    string env_block;
    LPVOID env_ptr = nullptr;
    if (!opts.env_set.empty())
    {
        env_block = buildEnvBlock(opts.env_set);
        env_ptr = (LPVOID)env_block.data();
    }

    /* DEBUG_PROCESS フラグ付きでプロセスを起動。
     * このプロセスがデバッガとなり、OutputDebugString を受信できる。*/
    PROCESS_INFORMATION pi = {};
    BOOL created = CreateProcessA(
        nullptr,
        cmd_buf.data(),
        nullptr, nullptr,
        TRUE,                                    /* ハンドル継承 */
        DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, /* このプロセスをデバッガとする */
        env_ptr,
        nullptr,
        &si, &pi);

    /* 子プロセス側のハンドルをクローズ (親では不要) */
    CloseHandle(stdin_r);
    CloseHandle(stdout_w);
    CloseHandle(stderr_w);

    if (!created)
    {
        CloseHandle(stdin_w);
        CloseHandle(stdout_r);
        CloseHandle(stderr_r);
        return result;
    }

    /* stdin に渡すデータを構築 (各行に \n を付けて結合) */
    string stdin_data;
    for (const auto &line : opts.stdin_lines)
    {
        stdin_data += line + "\n";
    }

    /* stdin 書き込み / stdout 読み込み / stderr 読み込みを並行実行。
     * デバッグイベントループはメインスレッドで動かすため、
     * I/O は別スレッドで処理してデッドロックを防ぐ。 */
    string stdout_out, stderr_out;

    thread stdin_thread([&]() {
        if (!stdin_data.empty())
        {
            DWORD written = 0;
            WriteFile(stdin_w, stdin_data.data(), (DWORD)stdin_data.size(), &written, nullptr);
        }
        CloseHandle(stdin_w);
    });

    thread stdout_thread([&]() {
        stdout_out = readAllHandle(stdout_r);
        CloseHandle(stdout_r);
    });

    thread stderr_thread([&]() {
        stderr_out = readAllHandle(stderr_r);
        CloseHandle(stderr_r);
    });

    /* デバッグイベントループ: OutputDebugString を逐次キャプチャする。
     * EXIT_PROCESS_DEBUG_EVENT を受信したらループを終了する。 */
    string debug_log;
    DWORD child_exit_code = (DWORD)-1;
    bool running = true;
    DWORD wait_timeout = (opts.timeout_ms > 0) ? (DWORD)opts.timeout_ms : INFINITE;

    while (running)
    {
        DEBUG_EVENT de = {};
        if (!WaitForDebugEvent(&de, wait_timeout))
        {
            /* タイムアウト: 子プロセスを強制終了 */
            TerminateProcess(pi.hProcess, (UINT)-1);
            break;
        }

        DWORD continue_status = DBG_CONTINUE;
        switch (de.dwDebugEventCode)
        {
        case OUTPUT_DEBUG_STRING_EVENT:
        {
            const auto &ods = de.u.DebugString;
            DWORD len = ods.nDebugStringLength;
            if (len > 0 && len <= 65536)
            {
                if (ods.fUnicode == 0)
                {
                    /* ANSI 文字列 */
                    vector<char> buf(len, '\0');
                    SIZE_T nread = 0;
                    if (ReadProcessMemory(pi.hProcess, ods.lpDebugStringData,
                                         buf.data(), len, &nread) &&
                        nread > 0)
                    {
                        debug_log.append(buf.data());
                    }
                }
                else
                {
                    /* Unicode 文字列 (UTF-16LE) → UTF-8 変換 */
                    vector<wchar_t> wbuf(len, L'\0');
                    SIZE_T nread = 0;
                    if (ReadProcessMemory(pi.hProcess, ods.lpDebugStringData,
                                         wbuf.data(), len * sizeof(wchar_t), &nread) &&
                        nread > 0)
                    {
                        int u8len = WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), -1,
                                                        nullptr, 0, nullptr, nullptr);
                        if (u8len > 0)
                        {
                            vector<char> u8buf(u8len, '\0');
                            WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), -1,
                                               u8buf.data(), u8len, nullptr, nullptr);
                            debug_log.append(u8buf.data());
                        }
                    }
                }
            }
            break;
        }

        case LOAD_DLL_DEBUG_EVENT:
            /* DLL ロードハンドルをクローズしてリソースリークを防ぐ */
            if (de.u.LoadDll.hFile != nullptr)
            {
                CloseHandle(de.u.LoadDll.hFile);
            }
            break;

        case EXIT_PROCESS_DEBUG_EVENT:
            child_exit_code = de.u.ExitProcess.dwExitCode;
            running = false;
            break;

        case EXCEPTION_DEBUG_EVENT:
            /* デバッグ開始時の初回ブレークポイント (INT 3) は DBG_CONTINUE で継続。
             * それ以外の例外はプロセス自身のハンドラに渡す。 */
            if (de.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT &&
                de.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_SINGLE_STEP)
            {
                continue_status = DBG_EXCEPTION_NOT_HANDLED;
            }
            break;

        default:
            break;
        }
        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, continue_status);
    }

    stdin_thread.join();
    stdout_thread.join();
    stderr_thread.join();

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    result.exit_code = (int)child_exit_code;
    result.stdout_out = stdout_out;
    result.stderr_out = stderr_out;
    result.debug_log = debug_log;

    return result;
}

#endif /* _WIN32 */
