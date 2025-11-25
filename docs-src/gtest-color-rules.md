# GoogleTest 実行結果の着色ルール

## 概要

GoogleTest は、テスト実行結果の視認性を高めるため、ターミナル出力に ANSI カラーコードを使用します。このドキュメントでは、出力行の見出し部 (角括弧で囲まれた部分) に適用される色を説明します。

## 色の定義

GoogleTest の内部では、以下の 4 つの色が定義されています。

| 色 | 内部定義名 | ANSI コード | 用途 |
|---|---|---|---|
| デフォルト (無色) | `kDefault` | `\033[0m` | 色のリセット |
| 緑 | `kGreen` | `\033[0;32m` | 成功状態の表示 |
| 赤 | `kRed` | `\033[0;31m` | 失敗状態の表示 |
| 黄 | `kYellow` | `\033[0;33m` | スキップ、警告の表示 |

## 見出し行と着色の対応

テスト実行中に表示される見出し行と、それぞれに適用される色を以下に示します。

### テスト実行フェーズ

| 見出し | 色 | 意味 |
|---|---|---|
| `[==========]` | 緑 | テスト全体の開始・終了 |
| `[----------]` | 緑 | テストスイートの開始・終了 |
| `[ RUN      ]` | 緑 | 個別テストの開始 |

### テスト結果

| 見出し | 色 | 意味 |
|---|---|---|
| `[       OK ]` | 緑 | 個別テストの成功 |
| `[  PASSED  ]` | 緑 | テスト全体の成功サマリ |
| `[  FAILED  ]` | 赤 | 個別テストの失敗、または失敗サマリ |
| `[ SKIPPED  ]` | 黄 | テストのスキップ (GTEST_SKIP 使用時) |

### その他の情報出力

| 見出し | 色 | 意味 |
|---|---|---|
| `[  DEATH   ]` | 黄 | Death test 実行中 |
| `[  INFO    ]` | 黄 | 情報メッセージ |

## 出力例

以下は、典型的なテスト実行結果の出力例です。

```text
[==========] Running 3 tests from 2 test suites.      <- 緑
[----------] Global test environment set-up.          <- 緑
[----------] 2 tests from FooTest                     <- 緑
[ RUN      ] FooTest.DoesAbc                          <- 緑
[       OK ] FooTest.DoesAbc (0 ms)                   <- 緑
[ RUN      ] FooTest.HasXyzProperty                   <- 緑
[  FAILED  ] FooTest.HasXyzProperty (1 ms)            <- 赤
[----------] 2 tests from FooTest (1 ms total)        <- 緑
[----------] 1 test from BarTest                      <- 緑
[ RUN      ] BarTest.SkippedOnCondition               <- 緑
[ SKIPPED  ] BarTest.SkippedOnCondition (0 ms)        <- 黄
[----------] 1 test from BarTest (0 ms total)         <- 緑
[----------] Global test environment tear-down        <- 緑
[==========] 3 tests from 2 test suites ran. (2 ms)   <- 緑
[  PASSED  ] 1 test.                                  <- 緑
[ SKIPPED  ] 1 test, listed below:                    <- 黄
[ SKIPPED  ] BarTest.SkippedOnCondition               <- 黄
[  FAILED  ] 1 test, listed below:                    <- 赤
[  FAILED  ] FooTest.HasXyzProperty                   <- 赤

 1 FAILED TEST
```

## 色出力の制御

色の有効・無効は、以下の方法で制御できます。

### コマンドラインオプション

```bash
./test_binary --gtest_color=yes   # 常に色を使用
./test_binary --gtest_color=no    # 色を使用しない
./test_binary --gtest_color=auto  # 自動判定 (デフォルト)
```

### 環境変数

```bash
export GTEST_COLOR=1    # 色を有効化
export GTEST_COLOR=0    # 色を無効化
```

### auto モードの判定条件

`--gtest_color=auto` (デフォルト) の場合、以下の条件をすべて満たすときに色が有効になります。

- 出力先がターミナル (TTY) である
- UNIX 系 OS の場合、環境変数 `TERM` が `xterm` または `xterm-color` に設定されている
- Windows の場合、コンソールがカラー出力をサポートしている

## ソースコード上の実装

着色は `gtest.cc` 内の `PrettyUnitTestResultPrinter` クラスで実装されています。主要なメソッドを以下に示します。

| メソッド | 出力する見出し |
|---|---|
| `OnTestProgramStart` | `[==========]` (緑) |
| `OnTestSuiteStart` | `[----------]` (緑) |
| `OnTestStart` | `[ RUN      ]` (緑) |
| `OnTestEnd` | `[       OK ]` (緑) または `[  FAILED  ]` (赤) または `[ SKIPPED  ]` (黄) |
| `OnTestProgramEnd` | `[  PASSED  ]` (緑)、`[  FAILED  ]` (赤) |

## 参考リンク

- [GoogleTest Advanced Topics - Colored Terminal Output](https://google.github.io/googletest/advanced.html)
- [GoogleTest ソースコード (gtest.cc)](https://github.com/google/googletest/blob/main/googletest/src/gtest.cc)
- [GTEST_SKIP によるテストスキップ機能 (PR #1544)](https://github.com/google/googletest/pull/1544)
