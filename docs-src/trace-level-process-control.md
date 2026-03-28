# プロセス制御関数への traceLevel サポート追加（設計検討資料）

## 背景

`testfw/libsrc/test_com/traceLevel.cc` は、テスト中に `setTraceLevel("関数名", TRACE_INFO/TRACE_DETAIL)`
でコンソール出力を制御する仕組みを提供している。
現在この仕組みはモック関数（`mock_calcHandler` 等）にのみ使われており、
プロセス制御関数群（`startProcessAsync`, `waitForOutput` 等）には適用されていない。

インテグレーションテスト（porter の `porterSendRecvTest` 等）では複数プロセスを起動・対話させるため、
テスト失敗時にどの操作がどのタイミングで行われたかを追跡しにくい。
traceLevel をプロセス制御関数にも対応させることで、テスト失敗時の原因調査を容易にする。

### traceLevel 機構の概要

| 関数 | 役割 |
|---|---|
| `resetTraceLevel()` | 辞書をクリアしてデフォルト値を `TRACE_NONE` にリセット |
| `setTraceLevel(const char* func, int level)` | 指定関数名のトレースレベルを設定 |
| `setDefaultTraceLevel(int level)` | 全関数共通のデフォルトレベルを設定 |
| `getTraceLevel()` | 現在の関数名 (`__func__`) をキーにしてレベルを取得するマクロ |

レベル定数:

| 定数 | 値 | 意味 |
|---|---|---|
| `TRACE_NONE` | 0 | 出力なし（デフォルト） |
| `TRACE_INFO` | 1 | 関数呼び出しと主要引数を出力 |
| `TRACE_DETAIL` | 2 | 戻り値・結果を追加出力 |

### 既存のモック関数での実装パターン

```cpp
int mock_calcHandler(int kind, int a, int b, int *result)
{
    // TRACE_NONE より大きければ基本情報を出力
    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d, %d, 0x%p", __func__, kind, a, b, (void *)result);
        // TRACE_DETAIL 以上なら戻り値も出力
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d, %d\n", *result, rtc);
        }
        else
        {
            printf("\n");
        }
    }
    // ...
}
```

テスト側での設定例:

```cpp
void SetUp() override
{
    resetTraceLevel();
    setTraceLevel("calcHandler", TRACE_DETAIL);
    setTraceLevel("add",         TRACE_DETAIL);
}
```

---

## 設計: 各関数のキーと振る舞い

### 対象関数と出力仕様

| 関数 | キー | TRACE_INFO の出力 | TRACE_DETAIL の追加出力 |
|---|---|---|---|
| `startProcessAsync` | `"startProcessAsync"` | `"  > startProcessAsync \"<path>\" \"<arg1>\" ..."` | 成功: `"  < startProcessAsync pid=<N>"`、失敗: `"  < startProcessAsync nullptr"` |
| `writeLineStdin` | `"writeLineStdin"` | `"  > writeLineStdin \"<line>\""` | なし |
| `waitForOutput` | `"waitForOutput"` | `"  > waitForOutput \"<pattern>\" timeout=<N>ms"` | 成功: `"  < waitForOutput matched"`、タイムアウト: `"  < waitForOutput timeout"` |
| `closeStdin` | `"closeStdin"` | `"  > closeStdin"` | なし |
| `interruptProcess` | `"interruptProcess"` | `"  > interruptProcess pid=<N>"` | なし |
| `killProcess` | `"killProcess"` | `"  > killProcess pid=<N>"` | なし |
| `waitProcess` | `"waitProcess"` | `"  > waitProcess timeout=<N>ms"` | `"  < waitProcess exit_code=<N>"` |

### 除外する関数

| 関数 | 除外理由 |
|---|---|
| `writeStdin` | `writeLineStdin` から呼ばれるため、両方に追加すると二重出力になる |
| `getStdout` / `getStderr` | 単純ゲッター。呼び出し頻度が高く、トレース出力の価値が低い |
| `getDebugLogCount` / `getDebugLog` | 同上 |

---

## 実装方針

### 修正ファイル

#### 1. `testfw/libsrc/test_com/processController_impl.h`

`getTraceLevel()` マクロと `TRACE_NONE`/`TRACE_DETAIL` 定数を使うため、
`#include <test_com.h>` を追加する。`processController*.cc` はすべてこのヘッダを include するため、
ここに 1 か所追加するだけで全実装ファイルに反映される。

```cpp
#include <processController.h>
#include <test_com.h>   // ← 追加
```

#### 2. `testfw/libsrc/test_com/processController.cc`（共通実装）

**`writeLineStdin`**:

```cpp
bool writeLineStdin(AsyncProcessHandle& handle, const string& line)
{
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s \"%s\"\n", __func__, line.c_str());
    }
    return writeStdin(handle, line + "\n");
}
```

**`waitForOutput`**（関数冒頭・throw 直前・return 直前に追加）:

```cpp
string waitForOutput(AsyncProcessHandle& handle,
                     const string& pattern,
                     int timeout_ms)
{
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s \"%s\" timeout=%dms\n", __func__, pattern.c_str(), timeout_ms);
    }

    // ... (既存の待機処理) ...

    size_t pos = handle->stdout_buf.find(pattern);
    if (pos == string::npos) {
        if (getTraceLevel() >= TRACE_DETAIL) {
            printf("  < %s timeout\n", __func__);
        }
        throw runtime_error(
            "waitForOutput: timeout or EOF before pattern: \"" + pattern + "\"");
    }

    if (getTraceLevel() >= TRACE_DETAIL) {
        printf("  < %s matched\n", __func__);
    }
    return handle->stdout_buf.substr(0, pos + pattern.size());
}
```

#### 3. `testfw/libsrc/test_com/processController_linux.cc`（Linux 固有）

**`startProcessAsync`**（引数検証後と各 return 直前に追加）:

```cpp
AsyncProcessHandle startProcessAsync(const string& path,
                                      const vector<string>& args,
                                      const ProcessOptions& opts)
{
    // preload_lib の検証後
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s \"%s\"", __func__, path.c_str());
        for (const auto& a : args) { printf(" \"%s\"", a.c_str()); }
        printf("\n");
    }

    // ... (既存の pipe / fork 処理) ...

    // 失敗時の return nullptr 直前
    if (getTraceLevel() >= TRACE_DETAIL) {
        printf("  < %s nullptr\n", __func__);
    }
    return nullptr;

    // ...

    // 成功時の return proc 直前
    if (getTraceLevel() >= TRACE_DETAIL) {
        printf("  < %s pid=%d\n", __func__, (int)proc->pid);
    }
    return proc;
}
```

**`interruptProcess`**:

```cpp
void interruptProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->pid == -1) { return; }
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s pid=%d\n", __func__, (int)handle->pid);
    }
    kill(handle->pid, SIGINT);
}
```

**`killProcess`**:

```cpp
void killProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->pid == -1) { return; }
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s pid=%d\n", __func__, (int)handle->pid);
    }
    kill(handle->pid, SIGKILL);
}
```

**`closeStdin`**:

```cpp
void closeStdin(AsyncProcessHandle& handle)
{
    if (!handle || handle->stdin_fd == -1) { return; }
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s\n", __func__);
    }
    close(handle->stdin_fd);
    handle->stdin_fd = -1;
}
```

**`waitProcess`**（関数冒頭と return 直前に追加）:

```cpp
int waitProcess(AsyncProcessHandle& handle, int timeout_ms)
{
    if (!handle) { return -1; }
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s timeout=%dms\n", __func__, timeout_ms);
    }

    // ... (既存の処理) ...

    handle->last_exit_code = exit_code;
    if (getTraceLevel() >= TRACE_DETAIL) {
        printf("  < %s exit_code=%d\n", __func__, exit_code);
    }
    return exit_code;
}
```

#### 4. `testfw/libsrc/test_com/processController_windows.cc`（Windows 固有）

Linux 版と同一の関数に同様のトレースを追加する。
`handle->pid` の型が `DWORD` であるため、フォーマット指定を `%lu` にする。

```cpp
// 例: interruptProcess (Windows)
void interruptProcess(AsyncProcessHandle& handle)
{
    if (!handle || handle->proc_handle == nullptr) { return; }
    if (getTraceLevel() > TRACE_NONE) {
        printf("  > %s pid=%lu\n", __func__, handle->pid);
    }
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, handle->pid);
}
```

---

## テストコードでの使用例

### 基本的な使い方

```cpp
void SetUp() override
{
    resetTraceLevel();
    setTraceLevel("startProcessAsync", TRACE_DETAIL);
    setTraceLevel("waitForOutput",     TRACE_DETAIL);
    setTraceLevel("writeLineStdin",    TRACE_INFO);
    setTraceLevel("waitProcess",       TRACE_DETAIL);
}
```

### 出力例

```
  > startProcessAsync "/path/to/recv" "config.conf" "1001"
  < startProcessAsync pid=12345
  > waitForOutput "受信待機中" timeout=5000ms
  < waitForOutput matched
  > startProcessAsync "/path/to/send" "config.conf" "1001"
  < startProcessAsync pid=12346
  > waitForOutput "メッセージ>" timeout=5000ms
  < waitForOutput matched
  > writeLineStdin "Hello Porter"
  > waitForOutput "圧縮送信しますか" timeout=3000ms
  < waitForOutput matched
  > writeLineStdin "N"
  > waitForOutput "続けて送信しますか" timeout=3000ms
  < waitForOutput matched
  > writeLineStdin "N"
  > waitProcess timeout=5000ms
  < waitProcess exit_code=0
```

---

## 実装後の検証方法

1. `make -C testfw` でビルドエラーがないことを確認
2. `test/porter/src/integration/porterSendRecvTest` の `SetUp()` に上記 `setTraceLevel` を追加して実行
3. テスト出力に `  > startProcessAsync ...` 等のトレースが表示されることを確認
4. `resetTraceLevel()` のみの状態（デフォルト）でトレース出力が出ないことを確認
