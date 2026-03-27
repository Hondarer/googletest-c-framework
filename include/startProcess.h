#ifndef _START_PROCESS_H
#define _START_PROCESS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

/* 構造体メンバーのアライメント調整パディングに対する警告を抑制する。
 * int と string の混在でパディングが生じるが、意図した設計のため無視する。 */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* _WIN32 */

namespace testing {

/** 非同期プロセスのハンドル。内部実装は非公開。 */
struct AsyncProcess;
using AsyncProcessHandle = shared_ptr<AsyncProcess>;

/** プロセス実行オプション (startProcess / startProcessAsync 共通) */
struct ProcessOptions {
    /** 追加または上書きする環境変数。 */
    map<string, string> env_set;

#ifndef _WIN32
    /** LD_PRELOAD に追加するライブラリの絶対パス (Linux のみ)。
     *  設定すると syslog モックが有効になり debug_log / getDebugLog() でキャプチャできる。
     *  testfw 提供: testfw/lib/$(TARGET_ARCH)/libmock_syslog.so */
    string preload_lib;
#endif

#ifdef _WIN32
    /** OutputDebugString 出力をキャプチャする (Windows のみ)。
     *  true にすると DEBUG_ONLY_THIS_PROCESS で起動し debug_log / getDebugLog() でキャプチャできる。
     *  Linux の preload_lib に相当する。デフォルト true (Linux の常時収集に合わせた既定値)。 */
    bool capture_debug_output = true;
#endif
};

/** プロセス実行結果 (startProcess() の返値) */
struct ProcessResult {
    int exit_code;     ///< 終了コード (-1 = 起動失敗またはタイムアウト)
    string stdout_out; ///< 標準出力
    string stderr_out; ///< 標準エラー出力
    /** Linux  : LD_PRELOAD した syslog モックの出力 (preload_lib 指定時のみ)
     *  Windows: OutputDebugString の出力 (capture_debug_output 指定時のみ) */
    string debug_log;
};

/**
 * プロセスを非同期で起動する。
 * stdin / stdout / stderr はすべてパイプ経由で制御される。
 *
 * @param path    実行ファイルのパス
 * @param args    コマンドライン引数 (argv[1] 以降)
 * @param opts    実行オプション (env_set / preload_lib 等)
 * @return        プロセスハンドル。起動失敗時は nullptr。
 */
extern AsyncProcessHandle startProcessAsync(
    const string& path,
    const vector<string>& args = {},
    const ProcessOptions& opts = ProcessOptions{});

/**
 * プロセスの stdin に文字列をそのまま書き込む (改行を付加しない)。
 *
 * @return  書き込み成功時は true。プロセスが終了済みの場合は false。
 */
extern bool writeStdin(AsyncProcessHandle& handle, const string& data);

/**
 * プロセスの stdin に 1 行書き込む (末尾に \n を付加する)。
 *
 * @return  書き込み成功時は true。プロセスが終了済みの場合は false。
 */
extern bool writeLineStdin(AsyncProcessHandle& handle, const string& line);

/**
 * stdout に指定パターンが出現するまで待機し、
 * それまでに受信したすべての出力を返す。
 *
 * タイムアウトした場合は std::runtime_error を送出する。
 *
 * @param pattern    待機する文字列 (部分一致)
 * @param timeout_ms タイムアウト (ms)。-1 で無制限。
 * @return           受信した stdout (パターン出現位置まで含む)
 */
extern string waitForOutput(AsyncProcessHandle& handle,
                             const string& pattern,
                             int timeout_ms = 5000);

/**
 * プロセスの stdin パイプを閉じる。
 * fgets ブロックに EOF を通知する用途で使用する。
 */
extern void closeStdin(AsyncProcessHandle& handle);

/**
 * プロセスに SIGINT (Ctrl+C 相当) を送る。
 * Linux  : kill(pid, SIGINT)
 * Windows: GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid)
 */
extern void interruptProcess(AsyncProcessHandle& handle);

/**
 * プロセスを強制終了する。
 * Linux  : kill(pid, SIGKILL)
 * Windows: TerminateProcess
 */
extern void killProcess(AsyncProcessHandle& handle);

/**
 * プロセス終了を待機し、終了コードを返す。
 *
 * @param timeout_ms タイムアウト (ms)。-1 で無制限。デフォルト 10000。
 * @return           終了コード。タイムアウト時は -1。
 */
extern int waitProcess(AsyncProcessHandle& handle, int timeout_ms = 10000);

/**
 * これまでに受信した stdout 全体を返す (非破壊)。
 */
extern string getStdout(AsyncProcessHandle& handle);

/**
 * これまでに受信した stderr 全体を返す (非破壊)。
 */
extern string getStderr(AsyncProcessHandle& handle);

/**
 * 現在の蓄積デバッグログの行数を返す。
 *
 * Linux では waitProcess() 後に一括収集されるため、waitProcess() 前は常に 0 を返す。
 * Windows では OutputDebugString 受信時にリアルタイム収集されるが、
 * 実用上は waitProcess() 後に参照することを推奨する。
 * ProcessOptions.preload_lib (Linux) / capture_debug_output (Windows) を
 * 指定しない場合は常に 0 を返す。
 */
extern size_t getDebugLogCount(AsyncProcessHandle& handle);

/**
 * 蓄積デバッグログを行単位のコレクションで返す (非破壊)。
 *
 * Linux  : LD_PRELOAD した libmock_syslog.so が一時ファイルに書き込んだ内容。
 *          waitProcess() 後に一括収集される。
 * Windows: OutputDebugString でキャプチャした内容 (capture_debug_output 指定時)。
 *          リアルタイム収集されるが、実用上は waitProcess() 後に参照することを推奨する。
 *
 * @param from_index  返却を開始する行インデックス (デフォルト 0 = 全件)。
 */
extern vector<string> getDebugLog(AsyncProcessHandle& handle, size_t from_index = 0);

/**
 * プロセスを起動し、終了まで待機して結果を返す。
 * startProcessAsync() のラッパーとして実装される。
 *
 * @param binary      実行するバイナリの絶対パス
 * @param args        コマンドライン引数 (argv[1] 以降)
 * @param opts        実行オプション (env_set / preload_lib 等)
 * @param stdin_lines stdin に渡す行リスト (各要素末尾に \n を付加して書き込む)。デフォルト空。
 * @param timeout_ms  タイムアウト (ms)。デフォルト 30000。
 * @return            実行結果
 */
inline ProcessResult startProcess(const string& binary,
                                   const vector<string>& args = {},
                                   const ProcessOptions& opts = ProcessOptions{},
                                   const vector<string>& stdin_lines = {},
                                   int timeout_ms = 30000)
{
    auto h = startProcessAsync(binary, args, opts);
    if (!h) {
        ProcessResult res;
        res.exit_code = -1;
        return res;
    }
    for (const auto& line : stdin_lines) { writeLineStdin(h, line); }
    closeStdin(h);
    ProcessResult res;
    res.exit_code  = waitProcess(h, timeout_ms);
    res.stdout_out = getStdout(h);
    res.stderr_out = getStderr(h);
    for (const auto& line : getDebugLog(h)) { res.debug_log += line; }
    return res;
}

} // namespace testing

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif /* _WIN32 */

#endif // _START_PROCESS_H
