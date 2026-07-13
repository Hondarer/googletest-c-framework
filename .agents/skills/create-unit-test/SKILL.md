---
name: create-unit-test
description: |
  app 配下の C テストを新規作成するときに使うスキルです。
  関数単体テストと main() を含むテストの作り分け、
  makepart.mk の設定、EXPECT_CALL と NiceMock の基本形を案内します。
when_to_use: |
  - app/<name>/test/src 配下で新しい C テストを追加するとき
  - 関数単体テストと main() テストの配置や書き方を確認したいとき
---

# app 向け C テスト作成

このスキルは `app/<name>/test/` 配下の Google Test 作成を対象にします。  
mock を新規追加する作業は `create-mock` を使い、この文書ではテスト コードと `makepart.mk` の作り方に絞ります。

## テストの種類と単体テストの位置付け

`app/<name>/test/` 配下には目的の異なる複数の種類のテストが含まれます。

| 種類 | 定義 | 配置先 |
|------|------|--------|
| **関数単体テスト** | テスト対象のソース ファイル 1 本のカバレッジを、依存関数の mock によって充足するテスト | `app/<name>/test/src/lib<lib>Test/<name>Test/` |
|  + **`main()` テスト** | `main()` を対象とする関数単体テストの一形態 | `app/<name>/test/src/.../<name>Test/` |
| **組み合わせテスト・総合テスト** | 複数のコンポーネントを実際にリンクして動作を検証するテスト | 対象に特化したディレクトリ |

**単体テストの核心**: mock が依存関数を差し替えるため、`subtract.c` のテストで `add.c` の実装は不要です。`makepart.mk` の `TEST_SRCS` に対象ファイル 1 本だけを指定し、依存する関数を `LIBS += mock_<lib>` で差し替えます。

組み合わせテスト・総合テストは、単体テストとは別に対象に特化したテストとして個別に用意します。このスキルが扱う主な対象は関数単体テストです。`main()` を対象とする場合も関数単体テストですが、`main()` という性質上の追加手続きがあります。

先に次を確認します。

- 対象の関数が `main()` かどうか (`main()` の場合は関数単体テストに追加の手続きが必要)
- 既存の近いテスト配置があるか
- 依存する関数の mock がすでにあるか
    - アプリケーション関数 (`calcHandler`, `add` 等) の mock が不足する場合は `create-mock` を使う
    - 標準ライブラリ関数 (`printf`, `fopen` 等) の mock が不足する場合は `create-testfw-mock` を使い、`framework/testfw/libsrc/` 配下にモックを追加する

参照先:

- `framework/testfw/docs/how-to-test.md`
- `framework/testfw/docs/how-to-expect.md`
- `framework/testfw/docs/about-test-phase.md`
- `docs/testing-tutorial.md`

## 配置の決め方

- 関数単体テストは `app/<name>/test/src/lib<lib>Test/<name>Test/` を優先します
- `main()` テストは `app/<name>/test/src/main/<name>Test/` を使います
- `makefile` は近い既存テストからそのまま揃え、差分は `makepart.mk` だけに入れます

`app/calc` の規範例:

- 関数単体: `app/calc/test/src/libcalcbaseTest/addTest/`
- `main()` テスト: `app/calc/test/src/main/addTest/`

## 関数単体テスト

規範例は `app/calc/test/src/libcalcbaseTest/addTest/addTest.cc` です。

方針:

- `#include <testfw.h>` を先頭に置きます
- 被テスト関数のヘッダーを直接 include します
- 依存先を差し替える必要がある場合だけ `Mock_<lib>` を生成し、`EXPECT_CALL` を書きます
- Act では被テスト関数を呼び出し、戻り値を `rtc` などの変数で受けます。Assert ではその変数と出力引数を確認し、被テスト関数の呼び出しを `EXPECT_*` / `ASSERT_*` (`EXPECT_CALL` を除く) の引数に直接書きません
- Arrange フェーズの前提ガード、SetUp / TearDown / fixture ヘルパー、フロー補助ヘルパーのガードは、Act の被テスト関数呼び出しではないため、この分離の対象外です。`ASSERT_NO_THROW` / `EXPECT_NO_FATAL_FAILURE` / `EXPECT_EXIT`、戻り値のない呼び出し、期待値側の補助計算も対象外です

`makepart.mk` の基本形:

```makefile
# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/calcbase/add.c
```

- `USE_WRAP_MAIN := 1` は入れません
- `LIBS += ...` は mock や補助ライブラリが必要なときだけ追加します
- `TEST_SRCS` はカバレッジ充足の対象とするソース ファイルだけを指定します。テスト固有の mock 実装などの補助ソースは `TEST_SRCS` に入れず `ADD_SRCS` に指定します

## main() テスト

`main()` テストは関数単体テストの一形態です。`makepart.mk` や `EXPECT_CALL` の基本方針は関数単体テストと同じです。`main()` である以上、テスト コードから直接 `main()` を呼び出せないため、以下の追加の手続きが必要です。

規範例は `app/calc/test/src/main/addTest/addTest.cc` を正とします。

追加の手続き:

- `USE_WRAP_MAIN := 1` を `makepart.mk` に指定し、`main()` を `__real_main()` として再公開します
- `argv` を組み立て、Act では `__real_main(argc, (char **)&argv)` を呼びます
- Assert では `main()` の戻り値を確認し、必要なら `printf` などの出力も検証します
- 出力確認があるときは `#include <mock_stdio.h>` を追加します
- `Mock_stdio` は未設定呼び出しを許容したいので `NiceMock<Mock_stdio>` を基本にします

`makepart.mk` の基本形:

```makefile
# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/add/add.c

# テスト対象のソース ファイルにある main() は直接実行されず、
# テスト コード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# ライブラリの指定
LIBS += mock_calcbase mock_libc com_util
```

- `USE_WRAP_MAIN := 1` を必ず入れます
- `mock_libc` は `mock_stdio.h` を使うなら通常必要です
- `__real_main` は `framework/testfw/include/wrapmain.h` から利用されます

## EXPECT_CALL の書き方

- 戻り値だけでなく、出力引数を書き換える必要があるときは `WillOnce([](...) { ... })` を使います
- 文字列出力の確認は `StrEq("...\n")` を使います
- 呼び出し回数だけを厳密に見たい場合は `Times(n)` を付けます
- 1 回しか使わない期待値は、不要に Arrange フェーズにて変数へ切り出さず `EXPECT_EQ(...)` へ即値で書きます。プラットフォームごとに値が違う場合は Assert 側を分岐します
- 共通の説明は `framework/testfw/docs/how-to-expect.md` を参照します

例:

```cpp
EXPECT_CALL(mock_calcbase, add(1, 2, _))
    .WillOnce([](int, int, int *result) {
        *result = 3;
        return CALC_SUCCESS;
    });

EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
    .WillOnce(DoDefault());
```

## プラットフォーム分岐

app 向けテストで OS 分岐が必要な場合は、`_WIN32` を直接使わず `app/<name>/prod/include/.../platform.h` の統一マクロを使います。  
被テスト ヘッダーが `platform.h` を読んでいない場合だけ、テスト コード側で明示的に include します。

```c
#if defined(PLATFORM_LINUX)
    /* Linux 向けテスト */
#elif defined(PLATFORM_WINDOWS)
    /* Windows 向けテスト */
#endif
```

- Linux / Windows の二択は `#if defined(PLATFORM_LINUX)` と `#elif defined(PLATFORM_WINDOWS)` を基本にし、Linux 側を先に記載します
- `#ifdef _WIN32` や `#ifndef _WIN32` を app test に持ち込まないようにします
- 既存テストに統一マクロの例がある場合は、その書き方に合わせます
- `results.log` 用のタグ付きコメントは、`#if` / `#elif` / `#else` の内側へ重複配置しません。共通化できる `[状態]` / `[Pre-Assert確認_*]` / `[Pre-Assert手順]` / `[確認_*]` は分岐の外に置きます
- 分岐内にタグ付きコメントを書くと、非アクティブ側の枝にある説明も evidence 生成で拾われ、多重に出力される場合があります
- 同名の `TEST_F` を複数の枝に重複定義しません。evidence 抽出は同名テストの全定義を連結するため、サマリーの全行が重複出力されます。本体が同一なら分岐を外して 1 本に統合し、片プラットフォーム固有のテストだけを単枝のガード (`#if defined(PLATFORM_WINDOWS)` 〜 `#endif`) で囲みます

## コメントの書き方

`framework/testfw/docs/about-test-phase.md` のフェーズ分割に従います。  
コメントはユーザーがテストの流れを理解するため、および、テスト エビデンス生成の入力になるため、既存例に合わせて各フェーズを明示し、コメントのみでどのようなテストをどのように行うかが把握できるようにします。  
各テスト ケースでは、フェーズごとに `// Arrange`、`// Pre-Assert`、`// Act`、`// Assert` のコメントを挿入します。

- 各テストの前にテストの概要を記載します (「〜ことの確認」。複数行でも構いません)
- Arrange フェーズの準備値には `[状態]`
- Pre-Assert フェーズの呼び出し期待には `[Pre-Assert確認_正常系]` / `[Pre-Assert確認_異常系]`
- Pre-Assert フェーズで呼び出された場合の動作には `[Pre-Assert手順]`
- Act フェーズの実行には `[手順]`
- Assert フェーズの検証には `[確認_正常系]` / `[確認_異常系]`

### 使用できるタグ

使用できるタグは次の 7 種類だけです。

| タグ | 用途 |
|------|------|
| `[状態]` | Arrange フェーズの準備値、前提条件 |
| `[Pre-Assert確認_正常系]` | mock への呼び出し期待の検証 (正常な動作パス) |
| `[Pre-Assert確認_異常系]` | mock への呼び出し期待の検証 (エラー処理パス) |
| `[Pre-Assert手順]` | mock が呼び出された場合の動作 |
| `[手順]` | Act フェーズの実行内容 |
| `[確認_正常系]` | 実行結果の検証 (正常な動作パス) |
| `[確認_異常系]` | 実行結果の検証 (エラー処理パス) |

- エビデンス生成 (`insert_summary_c_cpp.awk`) はタグ文字列の完全一致で抽出するため、`[手順_異常系]` や `[Pre-Assert手順_正常系]` のような亜種はエラーにならず、サマリーから黙って欠落します。上記 7 種類以外を使用しません
- サフィックスなしの `[確認]` / `[Pre-Assert確認]` は使用しません。`results.log` のサマリー見出しに「カテゴリ未指定」または区分のない件数 (`### 確認内容 (n)`) が出力された場合は修正対象です

### Pre-Assert 系タグの役割分離

`EXPECT_CALL` / `ON_CALL` などの Pre-Assert 設定では、確認と手順を分離して記載します。

- 呼び出されること、呼び出されないこと、呼び出される回数、引数の内容の検証は `[Pre-Assert確認_正常系]` / `[Pre-Assert確認_異常系]` に記載します
- 呼び出された場合に戻り値を返す、出力引数や変数を操作する動作 (`WillOnce` / `WillRepeatedly` の中身) は `[Pre-Assert手順]` に記載します
- mock の応答設定を `[確認_*]` や `[手順]` に記載しません。「〜を模擬する」「〜を返させる」は `[Pre-Assert手順]` の内容です

### 書式

- タグの直後に ` - 説明文` を続けます。説明が `- ` で始まる場合だけ確認件数として集計されます
- 説明文は、箇条書きとして読める簡潔な説明にします。簡潔であっても、処理の内容と等価な自然文であることが求められます。  
  たとえば、Arrange フェーズで Act フェーズで呼び出すパラメーターに値を設定しているのであれば、具体的な値を説明文にも記載します
- 説明文の文体はタグ種別ごとに統一します。いずれも体言止めにしません
    - `[状態]` は状態の説明として「〜とする。」「〜を用意する。」のように記載します
    - `[手順]` / `[Pre-Assert手順]` は「〜を行う。」「〜を呼び出す。」「〜を返却する。」のように動詞で終えます
    - `[確認_*]` / `[Pre-Assert確認_*]` は「〜であること。」「〜が返ること。」のように「こと。」で終えます
- 同じ説明文を複数のタグ行に複製しません。サマリーには同一行がそのまま複数回出力されます。複数の `EXPECT_CALL` が同じ趣旨でも、内容の異なる文にするか、代表 1 箇所だけにタグを置き他はタグなしの補足コメントとします
- プラットフォーム分岐がある場合、tag 付きコメントは原則として分岐の外へ置きます。分岐内には tag なしの補足コメントだけを置きます
- タグが実際に存在しないフェーズは、`results.log` 上で空欄になって構いません
- ただしフェーズ見出しそのものは省略せず、処理がなくても `// Arrange` `// Pre-Assert` `// Act` `// Assert` を順に記載します
- 正常系と異常系を区別して記載します
- フェーズ見出しは `// Arrange` `// Pre-Assert` `// Act` `// Assert` の形で統一します。`// Act & Assert` や `// Arrange & Act` のような結合見出しは使用しません

### テスト概要コメント

- `TEST_F` の直前に空行を挟まず連続して書いた `//` コメント行が、すべて `results.log` の概要 (description) として取り込まれます
- 概要は「〜ことの確認」で記載します。補足が必要な場合は複数行でも構いません
- 概要の途中に空行を挟みません。空行より前のコメント行は取り込まれず、概要が欠けます
- 概要として出力しないコメント (補助関数の説明など) は、空行を挟んで `TEST_F` から分離します

例:

```cpp
// Arrange
int argc = 3;
const char *argv[] = {"addTest", "1", "2"}; // [状態] - main() に与えるプログラム名を "addTest"、引数を、"1", "2" とする。

// Pre-Assert
EXPECT_CALL(mock_calcbase, add(1, 2, _))
    .WillOnce([](int, int, int *result) {
        *result = 3;
        return CALC_SUCCESS;
    }); // [Pre-Assert確認_正常系] - add(1, 2, &result) が 1 回呼び出されること。
        // [Pre-Assert手順] - add(1, 2, &result) にて result に 3 を設定し、CALC_SUCCESS を返す。

// Act
int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

// Assert
EXPECT_EQ(0, rtc); // [確認_正常系] - main() の戻り値が 0 であること。
```

## 実装時の確認項目

- 近い既存テストと同じディレクトリ構成に置かれていること
- `makefile` を編集せず `makepart.mk` だけで差分を表現していること
- 関数単体テストに `USE_WRAP_MAIN := 1` を入れていないこと
- `main()` テストで `USE_WRAP_MAIN := 1` を入れていること
- `EXPECT_CALL` が Act より前に書かれていること
- 被テスト関数の呼び出しが `EXPECT_*` / `ASSERT_*` (`EXPECT_CALL` を除く) の引数に埋め込まれていないこと
- 新しい mock が必要なら `create-mock` を使うこと
- `make test` 後に `results.log` のサマリーを確認し、次の状態になっていないこと
    - 「## テスト項目」ブロックが存在しない (タグが 1 つもない)
    - 「### 状態」「### 手順」が空欄なのに、実際には Arrange や Act で処理をしている
    - 「### 確認内容」の見出しに「カテゴリ未指定」または区分のない件数が出ている
    - 同一の説明文が複数回出力されている
