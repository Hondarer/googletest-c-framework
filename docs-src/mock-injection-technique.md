# 標準ライブラリへのモック注入テクニック

## 概要

本レポジトリでは、既存の C プログラムを変更することなく、標準ライブラリ関数（`stdio.h`, `stdlib.h`, `string.h` など）の呼び出しをモック関数に置き換える高度なテクニックを採用しています。

これにより、以下が実現できます：

- **非侵襲的なテスト**: テスト対象コードを一切変更せずにテスト可能
- **標準関数のモック化**: `fopen()`, `printf()`, `calloc()` などの標準関数の動作を制御
- **デバッグ情報の自動取得**: 呼び出し元のファイル名、行番号、関数名を自動的に記録
- **実行時の切り替え**: テスト中でも本物の実装とモックを柔軟に切り替え可能

## 全体アーキテクチャ

モック注入は以下の3層構造で実現されています：

```
[テスト対象Cコード]
    ↓ #include <stdio.h>
[include_override/stdio.h]  ← インクルードパスで優先される
    ↓ #include_next <stdio.h>  本物をインクルード
    ↓ #include <mock_stdio.h>  モック定義をインクルード
    ↓ #define fopen(...) mock_fopen(__FILE__, __LINE__, __func__, ...)
[mock_stdio.h]  ← マクロ定義とモッククラス定義
    ↓
[mock_fopen.cc]  ← モック実装（C++）
```

## 主要テクニック

### 1. ヘッダーインターセプション

`include_override/` ディレクトリに標準ライブラリと同名のヘッダーを配置し、コンパイル時に `-I include_override` を指定することで、標準ライブラリより優先的にインクルードさせます。

**ディレクトリ構成:**
```
include_override/
├── stdio.h       # stdio.h のインターセプター
├── stdlib.h      # stdlib.h のインターセプター
├── string.h      # string.h のインターセプター
├── unistd.h      # unistd.h のインターセプター
└── sys/
    ├── stat.h    # sys/stat.h のインターセプター
    └── wait.h    # sys/wait.h のインターセプター
```

### 2. #include_next による本物のインクルード

`#include_next` ディレクティブを使用して、インクルードパスの次の階層にある本物の標準ライブラリヘッダーをインクルードします。

**include_override/stdio.h の例:**
```c
#ifndef _OVERRIDE_STDIO_H
#define _OVERRIDE_STDIO_H

/* 本物を include */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include_next <stdio.h>
#pragma GCC diagnostic pop
#else // _WIN32
#pragma push_macro("_INC_STDIO")
#undef _INC_STDIO
#include <../ucrt/stdio.h>
#pragma pop_macro("_INC_STDIO")
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDIO_H
#include <mock_stdio.h>
#undef _IN_OVERRIDE_HEADER_STDIO_H

#endif // _OVERRIDE_STDIO_H
```

**ポイント:**
- Linux/Unix: `#include_next` で本物の `<stdio.h>` をインクルード
- Windows: UCRT（Universal C Runtime）から直接インクルード
- 警告抑制のための `#pragma` ディレクティブを使用

### 3. マクロによる関数置き換え

`mock_stdio.h` 内で、標準関数をマクロでモック関数に置き換えます。この際、`__FILE__`, `__LINE__`, `__func__` を自動的に付加します。

**include/mock_stdio.h の例:**
```c
#ifdef _IN_OVERRIDE_HEADER_STDIO_H

#define fclose(stream) mock_fclose(__FILE__, __LINE__, __func__, stream)
#define fflush(stream) mock_fflush(__FILE__, __LINE__, __func__, stream)
#define fopen(filename, modes) mock_fopen(__FILE__, __LINE__, __func__, filename, modes)
#define printf(format, ...) mock_printf(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define fprintf(stream, format, ...) mock_fprintf(__FILE__, __LINE__, __func__, stream, format, ##__VA_ARGS__)
#define fgets(s, n, stream) mock_fgets(__FILE__, __LINE__, __func__, s, n, stream)

#endif // _IN_OVERRIDE_HEADER_STDIO_H
```

**効果:**
- テスト対象コードで `fopen("test.txt", "r")` と書くと、自動的に `mock_fopen(__FILE__, __LINE__, __func__, "test.txt", "r")` に展開される
- 呼び出し元のファイル名、行番号、関数名が自動的に記録される

### 4. 二重定義の回避

`_IN_OVERRIDE_HEADER_*` マクロを使用して、Cコードとテストコード（C++）で異なる定義を使い分けます。

**役割分担:**

| コンテキスト | マクロ定義 | 効果 |
|------------|-----------|-----|
| テスト対象Cコード | `_IN_OVERRIDE_HEADER_STDIO_H` が定義される | マクロによる関数置き換えが有効 |
| テストコード（C++） | `_IN_OVERRIDE_HEADER_STDIO_H` が未定義 | Google Mock のクラス定義が有効 |

**include/mock_stdio.h の構造:**
```c
#ifdef _IN_OVERRIDE_HEADER_STDIO_H

// Cコード用：マクロ定義のみ
#define fopen(filename, modes) mock_fopen(__FILE__, __LINE__, __func__, filename, modes)

#else // _IN_OVERRIDE_HEADER_STDIO_H

// テストコード（C++）用：Google Mock クラス定義
#include <gmock/gmock.h>

extern FILE *delegate_real_fopen(const char *, const int, const char *, const char *, const char *);
extern FILE *delegate_fake_fopen(const char *, const int, const char *, const char *, const char *);

class Mock_stdio
{
public:
    MOCK_METHOD(FILE *, fopen, (const char *, const int, const char *, const char *, const char *));

    void switch_to_real_fileio();
    void switch_to_mock_fileio();

    Mock_stdio();
    ~Mock_stdio();
};

extern Mock_stdio *_mock_stdio;

#endif // _IN_OVERRIDE_HEADER_STDIO_H
```

### 5. Delegate パターンによる実装の切り替え

各モック関数には、以下の3つの実装が用意されています：

1. **mock_xxx**: エントリーポイント（グローバルモックインスタンスの有無で分岐）
2. **delegate_real_xxx**: 本物の実装を呼び出す
3. **delegate_fake_xxx**: フェイク実装（簡易的なスタブ）

**libsrc/mock_libc/mock_fopen.cc の例:**
```cpp
FILE *mock_fopen(const char *file, const int line, const char *func,
                 const char *filename, const char *modes)
{
    FILE *fp;

    if (_mock_stdio != nullptr)
    {
        // モックが有効な場合：Google Mock の期待値に基づいて動作
        fp = _mock_stdio->fopen(file, line, func, filename, modes);
    }
    else
    {
        // モックが無効な場合：本物の実装を呼び出す
        fp = delegate_real_fopen(file, line, func, filename, modes);
    }

    // トレース出力（デバッグ用）
    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > fopen %s, %c", filename, *modes);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> 0x%p\n", file, line, (void *)fp);
        }
        else
        {
            printf("\n");
        }
    }

    return fp;
}

FILE *delegate_real_fopen(const char *file, const int line, const char *func,
                          const char *filename, const char *modes)
{
    // 本物の fopen を呼び出す
#ifndef _WIN32
    return fopen(filename, modes);
#else
    FILE *fp = NULL;
    errno_t err = fopen_s(&fp, filename, modes);
    if (err != 0)
    {
        return NULL;
    }
    return fp;
#endif
}

FILE *delegate_fake_fopen(const char *file, const int line, const char *func,
                          const char *filename, const char *modes)
{
    // フェイク実装：ダミーのFILEポインタを返す
    FILE *fp = (FILE *)malloc(sizeof(FILE));
    return fp;
}
```

### 6. グローバルモックインスタンスによる制御

グローバル変数 `_mock_stdio` などを使用して、モックの有効/無効を制御します。

**制御フロー:**
```cpp
// テストフィクスチャのSetUp()で生成
Mock_stdio *_mock_stdio = nullptr;

void SetUp() override
{
    _mock_stdio = new Mock_stdio();  // モック有効化
}

void TearDown() override
{
    delete _mock_stdio;
    _mock_stdio = nullptr;  // モック無効化
}

// テスト実行中
// → _mock_stdio != nullptr のため、Google Mock が呼ばれる

// テスト終了後
// → _mock_stdio == nullptr のため、本物の実装が呼ばれる
```

## ビルド設定

モック注入を有効にするためには、以下のコンパイルオプションが必要です：

```makefile
# include_override を最優先のインクルードパスに追加
CFLAGS += -I$(TESTFW_DIR)/include_override
CFLAGS += -I$(TESTFW_DIR)/include

# モックライブラリをリンク
LDFLAGS += -L$(TESTFW_DIR)/lib
LDLIBS += -lmock_libc
```

**重要:** `-I$(TESTFW_DIR)/include_override` は、システムの標準インクルードパスよりも前に指定する必要があります。

## 実装例

### 完全な例：stdio.h のモック化

**1. include_override/stdio.h（インターセプター）**
```c
#ifndef _OVERRIDE_STDIO_H
#define _OVERRIDE_STDIO_H

/* 本物を include */
#ifndef _WIN32
#include_next <stdio.h>
#else
#include <../ucrt/stdio.h>
#endif

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDIO_H
#include <mock_stdio.h>
#undef _IN_OVERRIDE_HEADER_STDIO_H

#endif
```

**2. include/mock_stdio.h（モック定義）**
```c
#ifndef _MOCK_STDIO_H
#define _MOCK_STDIO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern FILE *mock_fopen(const char *, const int, const char *,
                       const char *, const char *);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_STDIO_H
// Cコード用：マクロ定義
#define fopen(filename, modes) mock_fopen(__FILE__, __LINE__, __func__, filename, modes)

#else
// C++テストコード用：Google Mock クラス定義
#include <gmock/gmock.h>

extern FILE *delegate_real_fopen(const char *, const int, const char *,
                                 const char *, const char *);

class Mock_stdio {
public:
    MOCK_METHOD(FILE *, fopen, (const char *, const int, const char *,
                                const char *, const char *));

    Mock_stdio();
    ~Mock_stdio();
};

extern Mock_stdio *_mock_stdio;

#endif

#endif
```

**3. libsrc/mock_libc/mock_stdio.cc（モッククラス実装）**
```cpp
#include <mock_stdio.h>

using namespace testing;

Mock_stdio *_mock_stdio = nullptr;

Mock_stdio::Mock_stdio()
{
    _mock_stdio = this;

    // デフォルトの動作を設定
    ON_CALL(*this, fopen(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_fopen));
}

Mock_stdio::~Mock_stdio()
{
    _mock_stdio = nullptr;
}
```

**4. libsrc/mock_libc/mock_fopen.cc（モック関数実装）**
```cpp
#include <mock_stdio.h>

FILE *mock_fopen(const char *file, const int line, const char *func,
                 const char *filename, const char *modes)
{
    if (_mock_stdio != nullptr)
    {
        return _mock_stdio->fopen(file, line, func, filename, modes);
    }
    else
    {
        return delegate_real_fopen(file, line, func, filename, modes);
    }
}

FILE *delegate_real_fopen(const char *file, const int line, const char *func,
                          const char *filename, const char *modes)
{
#ifndef _WIN32
    return fopen(filename, modes);
#else
    FILE *fp = NULL;
    fopen_s(&fp, filename, modes);
    return fp;
#endif
}
```

**5. テストコード（使用例）**
```cpp
#include <gtest/gtest.h>
#include <mock_stdio.h>

class MyTest : public ::testing::Test {
protected:
    Mock_stdio *mock_stdio;

    void SetUp() override {
        mock_stdio = new Mock_stdio();
    }

    void TearDown() override {
        delete mock_stdio;
    }
};

TEST_F(MyTest, TestFopenFailure)
{
    // fopen が失敗する期待値を設定
    EXPECT_CALL(*mock_stdio, fopen(_, _, _, "test.txt", "r"))
        .WillOnce(Return(nullptr));

    // テスト対象関数を呼び出す
    // （内部で fopen("test.txt", "r") が呼ばれる）
    int result = my_function_that_opens_file();

    // エラーハンドリングが正しく動作することを確認
    EXPECT_EQ(result, -1);
}

TEST_F(MyTest, TestFopenSuccess)
{
    FILE *fake_fp = (FILE *)0x12345678;

    // fopen が成功する期待値を設定
    EXPECT_CALL(*mock_stdio, fopen(_, _, _, "test.txt", "r"))
        .WillOnce(Return(fake_fp));

    int result = my_function_that_opens_file();

    EXPECT_EQ(result, 0);
}
```

## 利点

### 1. 非侵襲的
- テスト対象のCコードを一切変更する必要がない
- 依存性注入などのデザインパターンを強制しない
- レガシーコードにも適用可能

### 2. 詳細なトレース
- 呼び出し元のファイル名、行番号、関数名を自動的に記録
- デバッグ時に呼び出し元を特定しやすい

### 3. 柔軟性
- テスト中でも本物の実装とモックを切り替え可能
- 一部の関数だけモック化、他は本物を使用、といった柔軟な設定が可能

### 4. Google Mock との統合
- Google Mock の強力な機能（EXPECT_CALL, WillOnce, WillRepeatedly など）をそのまま利用可能

## 注意点とベストプラクティス

### 1. インクルードパスの順序

**必須:** `-I include_override` は最も優先度の高いインクルードパスに設定すること。

```makefile
# 正しい例
CFLAGS = -I./include_override -I./include -I/usr/include

# 誤った例（システムパスが優先されてしまう）
CFLAGS = -I/usr/include -I./include_override
```

### 2. マクロの衝突

標準関数をマクロで置き換えるため、以下の点に注意：

- 関数ポインタとして使用する場合は、マクロ展開を避ける必要がある
- `#undef` で一時的にマクロを無効化することも可能

```c
// 問題が発生する例
void (*fp)(const char *) = printf;  // マクロ展開されてコンパイルエラー

// 回避策
#undef printf
void (*fp)(const char *) = printf;
#define printf(format, ...) mock_printf(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
```

### 3. Windows と Linux の差異

- `#include_next` は GCC/Clang 拡張機能（MSVC では使用不可）
- Windows では UCRT のパスを直接指定する必要がある
- プラットフォーム固有の条件コンパイルを適切に使用すること

### 4. モックインスタンスのライフサイクル

- `_mock_stdio` などのグローバル変数は、テストフィクスチャの SetUp/TearDown で適切に管理すること
- テスト間でモックインスタンスが残らないように注意

### 5. 可変長引数関数

`printf` や `scanf` などの可変長引数関数をモック化する場合：

- マクロで `##__VA_ARGS__` を使用
- モック側で `va_list` を適切に処理

## まとめ

本レポジトリで採用している標準ライブラリへのモック注入テクニックは、以下の技術を組み合わせて実現されています：

1. **ヘッダーインターセプション**: インクルードパスの優先順位を利用
2. **#include_next**: 本物のヘッダーをインクルード
3. **マクロ置き換え**: 標準関数をモック関数に自動変換
4. **デバッグ情報の自動付加**: `__FILE__`, `__LINE__`, `__func__` の活用
5. **条件付きコンパイル**: CコードとC++テストコードの使い分け
6. **Delegateパターン**: 本物とフェイクの実装を動的に切り替え
7. **グローバルインスタンス**: モックの有効/無効を実行時制御

このテクニックにより、既存のCコードを一切変更することなく、標準ライブラリ関数の動作を完全に制御できます。これは、レガシーコードのテスタビリティを劇的に向上させる強力な手法です。

## 関連ドキュメント

- [How to mock](how-to-mock.md) - モック関数の追加手順
- [How to test](how-to-test.md) - テストの書き方
- [How to expect](how-to-expect.md) - 期待値の設定方法
