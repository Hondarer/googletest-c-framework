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

processController は全体で 1 つの機能であり、キーは `processController` 1 つとする。  
それぞれの機能で出力を以下のようにする。

| 機能 | TRACE_INFO の出力 | TRACE_DETAIL の追加出力 |
|---|---|---|
| `startProcessAsync` | `"  > startProcessAsync \"<path>\" \"<arg1>\" ..."` | 成功: `" -> startProcessAsync pid=<N>"`、失敗: `" -> startProcessAsync nullptr"` |
| `writeStdin` | `"  > writeStdin \"<string>\""` | なし |
| `writeLineStdin` | `"  > writeLineStdin \"<line>\""` | なし |
| `waitForOutput` | `"  > waitForOutput \"<pattern>\" timeout=<N>ms"` | 成功: `" -> waitForOutput matched"`、タイムアウト: `" -> waitForOutput timeout"` |
| `closeStdin` | `"  > closeStdin"` | なし |
| `interruptProcess` | `"  > interruptProcess pid=<N>"` | なし |
| `killProcess` | `"  > killProcess pid=<N>"` | なし |
| `waitForExit` | `"  > waitForExit timeout=<N>ms"` | `" -> waitForExit exit_code=<N>"` |
| stdout キャプチャ時( `\n` 検出時)           | `"  > stdout   : \"<line>\""` | なし |
| stderr キャプチャ時( `\n` 検出時)           | `"  > stderr   : \"<line>\""` | なし |
| debug出力 キャプチャ時( `\n` 検出時) | なし | `"  > debug_log: \"<line>\""` |

※ `writeStdin` は `writeLineStdin` からも呼ばれるため、最終出力処理を別関数 `writeStdinImpl` に逃がしてトレースさせる。

### 除外する関数

| 関数 | 除外理由 |
|---|---|
| `getStdout` / `getStderr` | 単純ゲッター。呼び出し頻度が高く、トレース出力の価値が低い |
| `getDebugLogCount` / `getDebugLog` | 同上 |

## テストコードでの使用例

### 基本的な使い方

```cpp
void SetUp() override
{
    resetTraceLevel();
    setTraceLevel("processController", TRACE_DETAIL);
}
```

---

## 実装後の検証方法

1. `make -C testfw` でビルドエラーがないことを確認
2. `test/override-sample/src/override-sampleTest` の `SetUp()` に上記 `setTraceLevel("processController", TRACE_DETAIL)` を追加
3. `test/porter/src/integration/porterSendRecvTest` の `SetUp()` に上記 `setTraceLevel("processController", TRACE_DETAIL)` を追加
4. テスト出力に `  > startProcessAsync ...` 等のトレースが表示されることを確認
5. `resetTraceLevel()` のみの状態（デフォルト）でトレース出力が出ないことを確認
