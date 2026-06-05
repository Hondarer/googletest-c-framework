---
name: create-testfw-mock
description: |
  testfw 配下の C ライブラリ関数 mock を作成するときに使うスキルです。
  include_override による差し替え、file/line/func 付きのシグネチャ、
  delegate_real_ と delegate_fake_、ON_CALL の既定動作をまとめます。
when_to_use: |
  - testfw/libsrc 配下で新しい mock 関数を追加するとき
  - testfw 向け mock の配置先や実装方針を確認したいとき
---

# testfw 向け C mock 作成

このスキルは testfw 配下の mock を対象にします。  
共通ルールは `docs/how-to-mock.md` を参照し、この文書では testfw 固有の構成と実装形を扱います。

## testfw mock の目的

testfw 配下の mock は、標準ライブラリ (libc 等) の関数をモック化します。目的は次の 2 つです。

- テスト コードが標準関数 (`printf`, `fopen`, `access` 等) を呼び出したかどうかを検証する
- テスト中に意図的な失敗を注入する (`fopen` が `NULL` を返す、`malloc` が失敗する など)

testfw mock は mock 未注入時に本物の関数へ委譲 (`delegate_real_`) するため、モックが注入されていないテストでもプログラムが正常に動作します。

規範実装は `libsrc/mock_libc/` を参照します。

## 方針

- testfw 向け mock は、mock 未注入時に `delegate_real_` で本物へ委譲します。
- mock 注入時の既定動作も `Invoke(delegate_real_<func>)` を基本にします。
- fake 実装が必要な関数には `delegate_fake_` を用意します。
- 置換マクロは `__FILE__`, `__LINE__`, `__func__` を追加して呼び出し元情報を渡します。
- `WEAK_ATR` は使いません。
- OS 差分は `_WIN32` / `#ifndef _WIN32` で明示します。

## 変更対象

- `include/mock_<lib>.h`
    - mock 関数宣言、置換マクロ、delegate 宣言、Mock クラスを定義します。
- `include_override/...`
    - 本物のヘッダーを読み込んだ後に置換マクロを有効化します。
- `libsrc/mock_<lib>/mock_<lib>.cc`
    - `ON_CALL` の既定値と `switch_to_real_*` / `switch_to_mock_*` を実装します。
- `libsrc/mock_<lib>/mock_<func>.cc`
    - `delegate_real_`、必要に応じて `delegate_fake_`、`mock_<func>` を実装します。

参照先:

- `include/mock_stdio.h`
- `include/mock_unistd.h`
- `include_override/stdio.h`
- `include_override/unistd.h`
- `libsrc/mock_libc/`

## ヘッダー構成

### mock 関数宣言と置換マクロ

```c
#ifdef __cplusplus
extern "C"
{
#endif

    extern int mock_access(const char *, const int, const char *, const char *, int);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_UNISTD_H
#define access(path, amode) mock_access(__FILE__, __LINE__, __func__, path, amode)
#else
extern int delegate_real_access(const char *, const int, const char *, const char *, int);
#endif
```

- override ヘッダー内では置換マクロを有効にします。
- override ヘッダー外では `delegate_real_` や `delegate_fake_` を宣言します。
- `printf` 系や `scanf` 系では `PRINTF_ATTR` / `SCANF_ATTR` も維持します。

### Mock クラス

```cpp
class Mock_unistd
{
public:
    MOCK_METHOD(int, access, (const char *, const int, const char *, const char *, int));
    MOCK_METHOD(pid_t, fork, (const char *, const int, const char *));

    Mock_unistd();
    ~Mock_unistd();
};
```

可変長引数は `MOCK_METHOD` に直接書かず、展開後の引数で表します。

```cpp
MOCK_METHOD(int, fprintf, (const char *, const int, const char *, FILE *, const char *));
MOCK_METHOD(int, scanf, (const char *, const int, const char *, const char *, va_list));
```

参照先:

- `include/mock_stdio.h`
- `include/mock_unistd.h`

## ON_CALL の既定値

### 基本形

```cpp
Mock_<lib>::Mock_<lib>()
{
    ON_CALL(*this, <func>(<matchers>))
        .WillByDefault(Invoke(delegate_real_<func>));

    _mock_<lib> = this;
}
```

mock が注入されていても、既定では本物と同じ動作にしておきます。

### 関連関数を一括で切り替える形

```cpp
Mock_stdio::Mock_stdio()
{
    switch_to_real_fileio();

    ON_CALL(*this, printf(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_printf));

    _mock_stdio = this;
}

void Mock_stdio::switch_to_mock_fileio()
{
    ON_CALL(*this, fclose(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_fclose));
}

void Mock_stdio::switch_to_real_fileio()
{
    ON_CALL(*this, fclose(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_fclose));
}
```

- ファイル I/O など、複数関数をまとめて切り替えたい場合に使います。
- 一括切り替えの対象外関数はコンストラクターに個別で残します。

参照先:

- `libsrc/mock_libc/mock_stdio.cc`

## 関数本体の実装

### 通常関数

```cpp
#include <test_com.h>
#include <mock_<lib>.h>

using namespace testing;

int delegate_real_<func>(const char *file, const int line, const char *func, <args>)
{
    (void)file;
    (void)line;
    (void)func;

    return <real_call>;
}

int mock_<func>(const char *file, const int line, const char *func, <args>)
{
    int rtc;

    if (_mock_<lib> != nullptr)
    {
        rtc = _mock_<lib>-><func>(file, line, func, <args>);
    }
    else
    {
        rtc = delegate_real_<func>(file, line, func, <args>);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > <func> <入力引数>");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
```

- 未注入時は `delegate_real_` へ進みます。
- `delegate_real_` では `file`, `line`, `func` を未使用として明示します。

参照先:

- `libsrc/mock_libc/mock_access.cc`
- `libsrc/mock_libc/mock_scanf.cc`

### fake 実装

`delegate_fake_` は、本物を呼ばずにテスト コードが安全に進む値を返します。

代表的な指針:

- リソース獲得系: `malloc` で一意なダミー資源を返す
- リソース解放系: `free` して成功値を返す
- 書き込み系: 実際には書き込まず、書き込み文字数相当を返す
- 読み込み系: `NULL` または空状態を返す
- その他の成否関数: `0` を返す

参照先:

- `libsrc/mock_libc/mock_fclose.cc`
- `libsrc/mock_libc/mock_fprintf.cc`

## 可変長引数

### printf 系

- mock 関数は元の `(..., const char *fmt, ...)` を保持します。
- Mock クラスと delegate では、展開済みの `const char *str` を受け取ります。
- `allocvprintf` が `NULL` の場合は失敗値を返します。

```cpp
int delegate_real_fprintf(const char *file, const int line, const char *func, FILE *stream, const char *str)
{
    (void)file;
    (void)line;
    (void)func;

    return fprintf(stream, "%s", str);
}
```

参照先:

- `libsrc/mock_libc/mock_fprintf.cc`
- `libsrc/mock_libc/mock_vfprintf.cc`

### scanf 系

- mock 関数は `va_list` を生成または受け取り、そのまま Mock クラスまたは `delegate_real_` へ渡します。
- `va_list` を複数回使う場合は `va_copy` を使います。

参照先:

- `libsrc/mock_libc/mock_scanf.cc`

## トレース出力

- `TRACE_INFO` では入力引数を出力します。
- `TRACE_DETAIL` では `from %s:%d` を追加して呼び出し元を出力します。
- `fprintf` 系の文字列は、末尾改行を除いた表示にします。
- `fclose` のように内部でポインター状態が変わる関数は、呼び出し前のアドレスを退避してから表示します。
- ポインター戻り値は `NULL` と非 `NULL` で表示を分けます。

参照先:

- `libsrc/mock_libc/mock_fprintf.cc`
- `libsrc/mock_libc/mock_fclose.cc`
- `libsrc/mock_libc/mock_fopen.cc`

## プラットフォーム分岐

- Linux 専用関数はファイル全体を `#ifndef _WIN32` で囲みます。
- Windows 専用関数は `#ifdef _WIN32` で囲みます。
- クロスプラットフォーム関数で実装だけが異なる場合は、関数内部で分岐します。Linux 側を先に記載します。

```c
#ifndef _WIN32
    /* Linux 専用 */
#endif /* !_WIN32 */
```

```c
#ifdef _WIN32
    /* Windows 専用 */
#endif /* _WIN32 */
```

```c
#ifndef _WIN32
    /* Linux 専用 */
#else /* !_WIN32 */
    /* Windows 専用 */
#endif /* _WIN32 */
```

参照先:

- `include/mock_unistd.h`
- `include_override/unistd.h`

## 実装時の確認項目

- 置換マクロに `__FILE__`, `__LINE__`, `__func__` が入っていること
- `delegate_real_` が本物の関数を呼んでいること
- fake 実装が資源の獲得と解放で整合していること
- `ON_CALL` の既定値が `Invoke(delegate_real_<func>)` になっていること
- `switch_to_mock_*` と `switch_to_real_*` の対象が対応していること
- `_WIN32` 条件分岐がヘッダーと実装の両方で整合していること
