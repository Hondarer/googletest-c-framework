#ifndef _RUN_PROCESS_H
#define _RUN_PROCESS_H

#include <map>
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

/** プロセス実行の結果 */
struct ProcessResult
{
    int exit_code;     ///< 終了コード (-1 = 起動失敗またはタイムアウト)
    string stdout_out; ///< 標準出力のキャプチャ
    string stderr_out; ///< 標準エラー出力のキャプチャ
    /** Linux : LD_PRELOAD した syslog モックの出力
     *  Windows: OutputDebugString の出力 */
    string debug_log;
};

/** プロセス実行オプション */
struct ProcessOptions
{
    /** コマンドライン引数 (argv[1] 以降) */
    vector<string> args;

    /** stdin に渡す行リスト。各要素に "\\n" を付けて結合したものを書き込む。
     *  ヒアドキュメントのように複数の入力をサポートする。*/
    vector<string> stdin_lines;

    /** 追加または上書きする環境変数。
     *  キーが既存の環境変数名と一致した場合は上書きする。*/
    map<string, string> env_set;

    /** タイムアウト (ミリ秒)。0 = 無制限。デフォルト 30000 */
    int timeout_ms;

#ifndef _WIN32
    /** LD_PRELOAD に追加するライブラリの絶対パス。
     *  空文字列の場合は LD_PRELOAD を設定しない。
     *  設定した場合、debug_log に syslog モックのファイル出力が格納される。
     *  testfw 提供の syslog モック: testfw/lib/$(TARGET_ARCH)/libmock_syslog.so
     *  (SYSLOG_MOCK_FILE 環境変数で出力先ファイルパスを受け取る。) */
    string preload_lib;
#endif

    ProcessOptions() : timeout_ms(30000)
    {
    }
};

/**
 * 指定したバイナリをサブプロセスとして実行し、
 * stdin / stdout / stderr と syslog(Linux) / OutputDebugString(Windows) を
 * キャプチャして返す。
 *
 * @param binary 実行するバイナリの絶対パス
 * @param opts   実行オプション
 * @return       実行結果 (exit_code, stdout_out, stderr_out, debug_log)
 */
extern ProcessResult runProcess(const string &binary, const ProcessOptions &opts = ProcessOptions());

} // namespace testing

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif /* _WIN32 */

#endif // _RUN_PROCESS_H
