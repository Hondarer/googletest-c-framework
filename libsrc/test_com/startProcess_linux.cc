#ifndef _WIN32

#include "startProcess_impl.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

namespace testing {

/* -------- AsyncProcess デストラクタ -------- */

AsyncProcess::~AsyncProcess()
{
    /* pid を先に取り出して -1 にする (二重 kill 防止) */
    pid_t my_pid = pid;
    pid = -1;

    if (my_pid != -1) {
        kill(my_pid, SIGKILL);
    }
    if (stdin_fd != -1) {
        close(stdin_fd);
        stdin_fd = -1;
    }
    /* reader_thread が動いている場合は join する。
     * プロセスを kill したので stdout/stderr パイプは EOF になり、
     * reader_thread は自然に終了する。 */
    if (reader_thread.joinable()) {
        reader_thread.join();
    }
    if (my_pid != -1) {
        waitpid(my_pid, nullptr, 0);
    }
    /* stdout_fd / stderr_fd は reader_thread が close 済み。
     * debug_log_fd はプロセス起動失敗時や reader_thread 未起動時に残る場合がある。 */
    if (debug_log_fd != -1) {
        close(debug_log_fd);
        debug_log_fd = -1;
    }
}

/* -------- startProcessAsync -------- */

AsyncProcessHandle startProcessAsync(const string& path,
                                      const vector<string>& args,
                                      const ProcessOptions& opts)
{
    if (!opts.preload_lib.empty() && access(opts.preload_lib.c_str(), F_OK) != 0) {
        fprintf(stderr, "startProcessAsync: preload_lib not found: %s\n",
                opts.preload_lib.c_str());
        return nullptr;
    }

    int stdin_pipe[2]  = {-1, -1};
    int stdout_pipe[2] = {-1, -1};
    int stderr_pipe[2] = {-1, -1};

    if (pipe(stdin_pipe) != 0 || pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0) {
        for (int fd : {stdin_pipe[0], stdin_pipe[1],
                       stdout_pipe[0], stdout_pipe[1],
                       stderr_pipe[0], stderr_pipe[1]}) {
            if (fd != -1) { close(fd); }
        }
        return nullptr;
    }

    auto proc = make_shared<AsyncProcess>();

    /* syslog キャプチャ用パイプ */
    int debug_log_pipe[2] = {-1, -1};
    if (!opts.preload_lib.empty()) {
        if (pipe(debug_log_pipe) != 0) {
            for (int pfd : {stdin_pipe[0], stdin_pipe[1],
                            stdout_pipe[0], stdout_pipe[1],
                            stderr_pipe[0], stderr_pipe[1]}) {
                close(pfd);
            }
            return nullptr;
        }
    }

    pid_t pid = fork();
    if (pid == -1) {
        for (int fd : {stdin_pipe[0], stdin_pipe[1],
                       stdout_pipe[0], stdout_pipe[1],
                       stderr_pipe[0], stderr_pipe[1],
                       debug_log_pipe[0], debug_log_pipe[1]}) {
            if (fd != -1) { close(fd); }
        }
        return nullptr;
    }

    if (pid == 0) {
        /* ----- 子プロセス ----- */
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdin_pipe[0],  STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        /* debug_log パイプ: read 端を閉じ、write 端の番号を環境変数で渡す */
        if (debug_log_pipe[0] != -1) { close(debug_log_pipe[0]); }
        if (debug_log_pipe[1] != -1) {
            char fd_str[16];
            snprintf(fd_str, sizeof(fd_str), "%d", debug_log_pipe[1]);
            setenv("SYSLOG_MOCK_FD", fd_str, 1);
            /* write 端は exec 後も継承 (FD_CLOEXEC 設定なし) */
        }

        for (const auto& kv : opts.env_set) {
            setenv(kv.first.c_str(), kv.second.c_str(), 1);
        }

        if (!opts.preload_lib.empty()) {
            const char* existing = getenv("LD_PRELOAD");
            string preload_val = opts.preload_lib;
            if (existing != nullptr && existing[0] != '\0') {
                preload_val += ":" + string(existing);
            }
            setenv("LD_PRELOAD", preload_val.c_str(), 1);
        }

        vector<char*> argv_vec;
        argv_vec.push_back(const_cast<char*>(path.c_str()));
        for (const auto& a : args) {
            argv_vec.push_back(const_cast<char*>(a.c_str()));
        }
        argv_vec.push_back(nullptr);

        execv(path.c_str(), argv_vec.data());
        _exit(127);
    }

    /* ----- 親プロセス ----- */
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);
    /* debug_log パイプ: write 端を閉じ、read 端を proc に格納 */
    if (debug_log_pipe[1] != -1) { close(debug_log_pipe[1]); }

    proc->pid          = pid;
    proc->stdin_fd     = stdin_pipe[1];
    proc->stdout_fd    = stdout_pipe[0];
    proc->stderr_fd    = stderr_pipe[0];
    proc->debug_log_fd = debug_log_pipe[0];

    /* ReaderThread: select() で stdout/stderr/debug_log を多重監視 */
    AsyncProcess* p = proc.get();
    proc->reader_thread = thread([p]() {
        char buf[4096];

        while (true) {
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = -1;
            if (p->stdout_fd != -1) {
                FD_SET(p->stdout_fd, &rfds);
                maxfd = std::max(maxfd, p->stdout_fd);
            }
            if (p->stderr_fd != -1) {
                FD_SET(p->stderr_fd, &rfds);
                maxfd = std::max(maxfd, p->stderr_fd);
            }
            if (p->debug_log_fd != -1) {
                FD_SET(p->debug_log_fd, &rfds);
                maxfd = std::max(maxfd, p->debug_log_fd);
            }
            if (maxfd == -1) { break; }

            int ret = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
            if (ret < 0) { break; }

            if (p->stdout_fd != -1 && FD_ISSET(p->stdout_fd, &rfds)) {
                ssize_t n = read(p->stdout_fd, buf, sizeof(buf));
                if (n > 0) {
                    lock_guard<mutex> lk(p->buf_mutex);
                    p->stdout_buf.append(buf, (size_t)n);
                    p->buf_cv.notify_all();
                } else {
                    close(p->stdout_fd);
                    p->stdout_fd = -1;
                }
            }
            if (p->stderr_fd != -1 && FD_ISSET(p->stderr_fd, &rfds)) {
                ssize_t n = read(p->stderr_fd, buf, sizeof(buf));
                if (n > 0) {
                    lock_guard<mutex> lk(p->buf_mutex);
                    p->stderr_buf.append(buf, (size_t)n);
                    p->buf_cv.notify_all();
                } else {
                    close(p->stderr_fd);
                    p->stderr_fd = -1;
                }
            }
            if (p->debug_log_fd != -1 && FD_ISSET(p->debug_log_fd, &rfds)) {
                ssize_t n = read(p->debug_log_fd, buf, sizeof(buf));
                if (n > 0) {
                    lock_guard<mutex> lk(p->buf_mutex);
                    p->debug_log_buf.append(buf, (size_t)n);
                    /* 改行で分割して debug_log_lines に追記 */
                    size_t pos;
                    while ((pos = p->debug_log_buf.find('\n')) != string::npos) {
                        p->debug_log_lines.push_back(
                            p->debug_log_buf.substr(0, pos + 1));
                        p->debug_log_buf.erase(0, pos + 1);
                    }
                } else {
                    close(p->debug_log_fd);
                    p->debug_log_fd = -1;
                }
            }
        }

        {
            lock_guard<mutex> lk(p->buf_mutex);
            p->process_done = true;
            p->buf_cv.notify_all();
        }
    });

    return proc;
}

/* -------- interruptProcess -------- */

void interruptProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->pid == -1) { return; }
    kill(handle->pid, SIGINT);
}

/* -------- killProcess -------- */

void killProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->pid == -1) { return; }
    kill(handle->pid, SIGKILL);
}

/* -------- writeStdin -------- */

bool writeStdin(AsyncProcessHandle& handle, const string& data)
{
    if (!handle || handle->stdin_fd == -1) { return false; }

    /* SIGPIPE をブロックして write 失敗時にシグナルで落ちないようにする */
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &mask, &oldmask);

    bool ok = true;
    const char* p = data.data();
    size_t remain = data.size();
    while (remain > 0) {
        ssize_t n = write(handle->stdin_fd, p, remain);
        if (n <= 0) { ok = false; break; }
        p      += n;
        remain -= (size_t)n;
    }

    pthread_sigmask(SIG_SETMASK, &oldmask, nullptr);
    return ok;
}

/* -------- closeStdin -------- */

void closeStdin(AsyncProcessHandle& handle)
{
    if (!handle || handle->stdin_fd == -1) { return; }
    close(handle->stdin_fd);
    handle->stdin_fd = -1;
}

/* -------- waitProcess -------- */

int waitProcess(AsyncProcessHandle& handle, int timeout_ms)
{
    if (!handle) { return -1; }

    /* 二重呼び出し時は cached 終了コードを返す */
    if (handle->pid == -1 && !handle->reader_thread.joinable()) {
        return handle->last_exit_code;
    }

    /* stdin を閉じて子プロセスに EOF を通知 */
    if (handle->stdin_fd != -1) {
        close(handle->stdin_fd);
        handle->stdin_fd = -1;
    }

    /* process_done を条件変数で待機 (= stdout/stderr が EOF になるまで) */
    {
        unique_lock<mutex> lk(handle->buf_mutex);
        if (timeout_ms < 0) {
            handle->buf_cv.wait(lk, [&] { return handle->process_done; });
        } else {
            auto deadline = chrono::steady_clock::now() + chrono::milliseconds(timeout_ms);
            if (!handle->buf_cv.wait_until(lk, deadline, [&] { return handle->process_done; })) {
                /* タイムアウト: プロセスを強制終了 */
                lk.unlock();
                if (handle->pid != -1) {
                    kill(handle->pid, SIGKILL);
                }
            }
        }
    }

    if (handle->reader_thread.joinable()) {
        handle->reader_thread.join();
    }

    int exit_code = -1;
    pid_t my_pid = handle->pid;
    handle->pid = -1;

    if (my_pid != -1) {
        int status = 0;
        waitpid(my_pid, &status, 0);
        if (WIFEXITED(status)) {
            exit_code = WEXITSTATUS(status);
        }
    }

    /* debug_log_lines は reader_thread がパイプ経由でリアルタイム収集済み */

    handle->last_exit_code = exit_code;
    return exit_code;
}

} // namespace testing

#endif /* _WIN32 */
