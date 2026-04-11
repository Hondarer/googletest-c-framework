# プロセス制御 API

## 概要

`framework/testfw/include/processController.h` が提供するプロセス制御 API です。
テスト対象バイナリをサブプロセスとして起動し、
stdin / stdout / stderr をパイプ経由で制御できます。

2 つのモードがあります。

| モード | API | 用途 |
|---|---|---|
| 同期 | `startProcess()` | 短命プロセス (起動 → 終了まで待機) |
| 非同期 | `startProcessAsync()` + 各操作関数 | 常駐プロセス・対話プロセスの制御 |

`startProcess()` は `startProcessAsync()` のラッパーとして実装されており、
実質的な処理はすべて非同期 API 側に集約されています。

## 型・構造体

### AsyncProcessHandle

```cpp
struct AsyncProcess;
using AsyncProcessHandle = shared_ptr<AsyncProcess>;
```

非同期プロセスのハンドルです。内部実装は非公開です。
`shared_ptr` であるため、スコープアウト時にデストラクタで自動的にリソースが解放されます。

### ProcessOptions

```cpp
struct ProcessOptions {
    map<string, string> env_set;

#ifndef _WIN32
    string preload_lib;
#endif

#ifdef _WIN32
    bool capture_debug_output = true;
#endif
};
```

| フィールド | 説明 |
|---|---|
| `env_set` | 追加または上書きする環境変数 |
| `preload_lib` | LD_PRELOAD に追加するライブラリの絶対パス **(Linux のみ)**。`framework/testfw/lib/$(TARGET_ARCH)/libmock_syslog.so` を指定すると syslog 出力を `getDebugLog()` でキャプチャできる |
| `capture_debug_output` | OutputDebugString 出力をキャプチャする **(Windows のみ)**。`true` にすると `getDebugLog()` でキャプチャできる。Linux の `preload_lib` に相当する。**デフォルト `true`** |

### ProcessResult

`startProcess()` の返値です。

```cpp
struct ProcessResult {
    int    exit_code;     // 終了コード (-1 = 起動失敗またはタイムアウト)
    string stdout_out;    // 標準出力
    string stderr_out;    // 標準エラー出力
    string debug_log;     // デバッグログ出力 (preload_lib / capture_debug_output 指定時のみ)
};
```

`debug_log` は以下の場合に内容が入ります。

- Linux: `preload_lib` を指定した場合 (syslog モック出力)
- Windows: `capture_debug_output = true` を指定した場合 (OutputDebugString 出力)

## API リファレンス

### 同期 API

#### startProcess

```cpp
inline ProcessResult startProcess(
    const string& binary,
    const vector<string>& args        = {},
    const ProcessOptions& opts        = ProcessOptions{},
    const vector<string>& stdin_lines = {},
    int timeout_ms                    = 30000)
```

プロセスを起動し、終了まで待機して `ProcessResult` を返します。

| 引数 | 説明 |
|---|---|
| `binary` | 実行するバイナリの絶対パス |
| `args` | コマンドライン引数 (argv[1] 以降) |
| `opts` | 実行オプション |
| `stdin_lines` | stdin に渡す行リスト。各要素末尾に `\n` を付加して順次書き込む |
| `timeout_ms` | タイムアウト (ms)。デフォルト 30000 |

処理フロー:

1. `startProcessAsync()` でプロセスを起動
2. `stdin_lines` を `writeLineStdin()` で順次書き込み
3. `closeStdin()` で EOF を通知
4. `waitForExit(handle, timeout_ms)` で終了待機
5. `getStdout()` / `getStderr()` / `getDebugLog()` で結果を収集して返す

### 非同期 API

#### startProcessAsync

```cpp
extern AsyncProcessHandle startProcessAsync(
    const string& path,
    const vector<string>& args = {},
    const ProcessOptions& opts = ProcessOptions{})
```

プロセスを非同期で起動します。
stdin / stdout / stderr はすべてパイプ経由で制御されます。
起動失敗時は `nullptr` を返します。

#### writeStdin / writeLineStdin

```cpp
extern bool writeStdin    (AsyncProcessHandle& handle, const string& data)
extern bool writeLineStdin(AsyncProcessHandle& handle, const string& line)
```

プロセスの stdin に書き込みます。
`writeStdin` は改行を付加しません。`writeLineStdin` は末尾に `\n` を付加します。
プロセスが終了済みの場合は `false` を返します。

#### waitForOutput

```cpp
extern string waitForOutput(AsyncProcessHandle& handle,
                             const string& pattern,
                             int timeout_ms = 5000)
```

stdout に指定パターン (部分一致) が出現するまで待機します。
パターン出現位置までの stdout を返します。
タイムアウトした場合、またはプロセスが予期せず終了した場合は `std::runtime_error` を送出します。

stdout の読み取りはバックグラウンドスレッドが常時行っているため、
`writeLineStdin()` と同時に呼び出してもデッドロックは発生しません。

Linux / Windows いずれも reader_thread がリアルタイムで収集するため、`waitForOutput()` 完了後にはその時点までの全ログが利用可能です。

> **テスト対象側の要件**: stdout / stderr はパイプ経由で受け渡されるため、
> テストが待機するパターンを出力した直後にテスト対象側で `fflush(stdout)` を呼ぶ必要があります。
> 呼ばれていない場合、出力が stdio の内部バッファに滞留し、タイムアウトまでパターンが届きません。

#### closeStdin

```cpp
extern void closeStdin(AsyncProcessHandle& handle)
```

stdin パイプを閉じます。`fgets()` でブロックしているプロセスに EOF を通知します。

#### interruptProcess

```cpp
extern void interruptProcess(AsyncProcessHandle& handle)
```

プロセスに Ctrl+C 相当のシグナルを送ります。

- Linux: `kill(pid, SIGINT)`
- Windows: `GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid)`

#### killProcess

```cpp
extern void killProcess(AsyncProcessHandle& handle)
```

プロセスを強制終了します。

- Linux: `kill(pid, SIGKILL)`
- Windows: `TerminateProcess`

#### waitForExit

```cpp
extern int waitForExit(AsyncProcessHandle& handle, int timeout_ms = 10000)
```

プロセス終了を待機し、終了コードを返します。
タイムアウト時は `-1` を返します。

Linux / Windows いずれも reader_thread がリアルタイムで収集するため、`waitForExit()` 完了後には全ログが利用可能です。

#### getStdout / getStderr

```cpp
extern string getStdout(AsyncProcessHandle& handle)
extern string getStderr(AsyncProcessHandle& handle)
```

これまでに受信した stdout / stderr 全体を返します (非破壊)。

#### getDebugLogCount / getDebugLog

```cpp
extern size_t          getDebugLogCount(AsyncProcessHandle& handle)
extern vector<string>  getDebugLog     (AsyncProcessHandle& handle, size_t from_index = 0)
```

デバッグログを行単位で返します (非破壊)。

- **Linux**: `preload_lib` を指定した場合に有効。syslog モック出力が対象。
- **Windows**: `capture_debug_output = true` を指定した場合に有効。OutputDebugString 出力が対象。
- `from_index` に `getDebugLogCount()` で記録したインデックスを渡すと、
  その時点以降のログのみを取り出せます。

## 使い方

### 同期 (startProcess)

```cpp
// 最小呼び出し
ProcessResult res = startProcess(binary_path);

// 引数・環境変数・入力行を指定
ProcessOptions opts;
opts.env_set["LD_LIBRARY_PATH"] = lib_path;

ProcessResult res = startProcess(
    binary_path,
    {"config.conf", "10"},  // args
    opts,
    {"Hello", "N", "N"},    // stdin_lines
    5000);                  // timeout_ms

EXPECT_EQ(0, res.exit_code);
EXPECT_NE(string::npos, res.stdout_out.find("送信完了"));
```

### 非同期 (startProcessAsync)

```cpp
// 常駐プロセスを起動して対話する例
ProcessOptions opts;
opts.env_set["LD_LIBRARY_PATH"] = lib_path;

AsyncProcessHandle recv_h = startProcessAsync(recv_path, {config_path, "10"}, opts);
ASSERT_NE(nullptr, recv_h);

// リスニング開始を確認してから次のステップへ
ASSERT_NO_THROW(waitForOutput(recv_h, "受信待機中", 5000));

// 別プロセスを起動して対話する
AsyncProcessHandle send_h = startProcessAsync(send_path, {config_path, "10"}, opts);
ASSERT_NO_THROW(waitForOutput(send_h, "メッセージ>", 5000));

ASSERT_TRUE(writeLineStdin(send_h, "Hello Porter"));
ASSERT_NO_THROW(waitForOutput(send_h, "圧縮送信しますか", 3000));
ASSERT_TRUE(writeLineStdin(send_h, "N"));
ASSERT_NO_THROW(waitForOutput(send_h, "続けて送信しますか", 3000));
ASSERT_TRUE(writeLineStdin(send_h, "N"));
EXPECT_EQ(0, waitForExit(send_h, 5000));

// 常駐プロセスを SIGINT で停止する
interruptProcess(recv_h);
waitForExit(recv_h, 3000);

EXPECT_NE(string::npos, getStdout(recv_h).find("Hello Porter"));
```

### テスト失敗時のプロセスリーク防止

`ASSERT_*` マクロでテストが中断された場合でもプロセスを確実に終了させるため、
`TearDown()` に終了処理を実装します。

```cpp
void TearDown() override {
    if (send_h_) { killProcess(send_h_); waitForExit(send_h_, 1000); }
    if (recv_h_) { killProcess(recv_h_); waitForExit(recv_h_, 1000); }
}
```

### デバッグログのキャプチャ

```cpp
ProcessOptions opts = makeOpts();
#ifndef _WIN32
opts.preload_lib = ws + "/framework/testfw/lib/" + TARGET_ARCH + "/libmock_syslog.so";
#endif
/* Windows は capture_debug_output がデフォルト true のため追加設定不要 */

AsyncProcessHandle h = startProcessAsync(binary, args, opts);
ASSERT_NO_THROW(waitForOutput(h, "起動完了", 5000));

interruptProcess(h);
waitForExit(h, 3000); // ← Linux はここでログが一括収集される

// waitForExit() 後にログを検証
auto logs = getDebugLog(h);
EXPECT_TRUE(any_of(logs.begin(), logs.end(),
    [](const string& l) { return l.find("received message") != string::npos; }));
```

> **注意**: Linux の `getDebugLogCount()` は `waitForExit()` 後にのみ意味のある値を返します。
> ステップ別のログ分割は `waitForExit()` 後に `from_index` で行ってください。

## 旧 API (runProcess) からの移行

| 旧 | 新 |
|---|---|
| `runProcess()` | `startProcess()` |

`ProcessOptions` から `args`・`timeout_ms`・`stdin_lines` フィールドが削除されました。
それぞれ `startProcess()` の独立した引数として渡します。

```cpp
// 旧 API
ProcessOptions opts;
opts.args         = {"config.conf", "10"};
opts.stdin_lines  = {"Hello", "N"};
opts.timeout_ms   = 5000;
opts.env_set["LD_LIBRARY_PATH"] = lib_path;
auto res = runProcess(binary, opts);

// 新 API
ProcessOptions opts;
opts.env_set["LD_LIBRARY_PATH"] = lib_path;
auto res = startProcess(binary, {"config.conf", "10"}, opts, {"Hello", "N"}, 5000);
```
