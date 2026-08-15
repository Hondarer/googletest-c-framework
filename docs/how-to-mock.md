# How to mock

mock 関数を追加するときの共通ルールを示します。

## 用語

- mock
    - ここではテスト ダブル全般を指す総称として扱います。
- override ヘッダー
    - 本物のヘッダーを読み込んだ後に、関数呼び出しを mock へ差し替えるためのヘッダーです。
    - 置換マクロが注入されるのは、ベース名が `TEST_SRCS` に一致するソース (makefw の `makesrc_c_cpp.mk` が `CFLAGS_TEST` / `CXXFLAGS_TEST` + `-D_IN_TEST_SRC` でコンパイルするテスト対象) だけです。テストの `.cc`、mock ライブラリ、`ADD_SRCS` には適用されません。
    - このため `read` / `write` / `close` のような一般名の CRT 関数も、C++ テスト コードとの名前衝突を心配せずにマクロで mock 化できます。
- Mock クラス
    - Google Mock の `MOCK_METHOD` を並べたクラスです。テスト中の振る舞いはここで定義します。
- mock 関数本体
    - 置換マクロの着地点になる関数です。グローバル ポインター経由で Mock クラスへ委譲します。

## 流れ

1. Mock クラスへ `MOCK_METHOD` を追加します。
2. Mock クラスのコンストラクター、または切り替えメソッドへ `ON_CALL` の既定動作を追加します。
3. 関数本体を実装し、Mock クラスまたは既定処理へ流れる経路を作ります。

## 命名規則

- mock ヘッダー: `mock_<module>.h` または `mock_<lib>.h`
- override ヘッダー: 本物と同じヘッダー名
- 置換先関数名: 対象ライブラリの既存パターンに従います
- 既定処理: `delegate_real_<func>` / `delegate_fake_<func>` など
- グローバル ポインター: `_mock_<lib>`
- Mock クラス: `Mock_<lib>`
- テスト内の Mock オブジェクト変数: 型名をすべて小文字にした識別子 (`Mock_com_util` → `mock_com_util`、`Mock_stdio` → `mock_stdio`)

命名は既存ライブラリと揃えます。新規命名を持ち込まず、同一ライブラリ内の既存パターンに合わせてください。  
変数名 `mock_` や `mock` は、どの Mock クラスかを表さないため使いません。  
判定手順と検証コマンドは [コーディング規範](../../../app/general/docs/coding-guideline.md) の「テストのモック オブジェクト変数」を正とします。

### 試験側の戻り値中継

結果コード以外の戻り値も対象にする点は同じですが、場所で名前を分けます。

| 場所 | 変数 | 役割 |
|---|---|---|
| mock 関数本体 | `mock_ret` | 委譲先の値を受けて返す |
| テスト本体 | `actual_ret` | 試験対象の実際の戻り値を `EXPECT_*` で見る |

どちらも生産コードの `ret` とは混ぜません。

mock 関数本体は、意味論よりもテンプレート性を優先し、経路に応じて次の 2 形のどちらかに揃えます。

if / else の双方で委譲先を代入する場合は、初期化子を付けません。  
ダミーの初期化子を付けると、代入漏れの経路をコンパイラ警告で見つけにくくなります。

```cpp
T mock_ret;

if (_mock_sample != nullptr)
{
    mock_ret = _mock_sample->sample_func(...);
}
else
{
    mock_ret = delegate_real_sample_func(...);
}

return mock_ret;
```

既定値を置き、mock が無いときはその値を返す場合は、宣言時の初期化子で既定値を与えます。  
宣言の直後に同じ定数を代入する行は書きません。

```cpp
T mock_ret = 0;

if (_mock_sample != nullptr)
{
    mock_ret = _mock_sample->sample_func(...);
}

return mock_ret;
```

テスト本体は、エビデンス用に呼び出しと同じ行で宣言して構いません。

```cpp
int actual_ret = sample_func(...); // [手順]
EXPECT_EQ(SAMPLE_OK, actual_ret);  // [確認]
```

同一テスト関数で複数の戻り値を残すときは、`actual_ret_<区別>` とします。  
`rtc_` 接頭辞は使いません。

```cpp
int actual_ret_open = sample_open(...); // [手順]
int actual_ret_read = sample_read(...); // [手順]
```

テスト本体に `mock_ret` は使いません。そこは mock の中継ではなく、実際の戻り値です。  
`rtc`、`rtc_*`、`ret`、同じ役の `result` も使いません。

対象外は次のとおりです。

- 本物 API の出力引数名 (`gmtime_r` の `struct tm *result` など)
- 生産コードの結果コード変数 (規範本則の `ret` / `result`)

## Mock クラスの追加

`MOCK_METHOD` に、テストから制御したい関数を追加します。

```cpp
MOCK_METHOD(int, sample_func, (int, const char *));
```

- 引数なし関数は空の `()` を使います。
- 可変長引数は、Mock クラスで直接扱える形へ変換して宣言します。

## 注入ライフサイクル

テスト Fixture やテスト本体で `Mock_<lib>` を生成すると、そのコンストラクターで `_mock_<lib>` が現在のオブジェクトを指します。  
スコープを抜けるとデストラクターで `nullptr` に戻ります。

同じ `Mock_<lib>` クラスのオブジェクトを同時に複数生成してはいけません。  
mock 関数は `_mock_<lib>` が指す一つのオブジェクトへ委譲するため、複数生成すると期待値を設定したオブジェクトと実際の委譲先が一致しなくなります。  
コンストラクターとデストラクターでは、登録ポインターを直接代入せず、testfw の登録マクロを使用します。

```cpp
Mock_sample::Mock_sample()
{
    TESTFW_REGISTER_MOCK_INSTANCE(_mock_sample);
}

Mock_sample::~Mock_sample()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_sample);
}
```

`TESTFW_REGISTER_MOCK_INSTANCE` はクラスごとの生成数を検査し、同じクラスの 2 個目が存在する場合は Google Test の非致命的失敗を記録します。  
多重生成を検出した場合も、先に生成されたオブジェクトの登録を維持します。  
テストから `_mock_<lib>` を直接設定または解除してはいけません。

```cpp
TEST_F(MyTest, example)
{
    Mock_sample mock_sample;

    EXPECT_CALL(mock_sample, sample_func(_, _)).Times(1);

    /* テスト対象コード */
}
```

この構造により、mock の注入と解放はオブジェクトの生存期間に従います。

## 実装時の共通確認項目

- 置換対象の関数と `MOCK_METHOD` のシグネチャが一致していること
- `ON_CALL` の既定動作が追加されていること
- `_mock_<lib>` の設定と解除に `TESTFW_REGISTER_MOCK_INSTANCE` と `TESTFW_UNREGISTER_MOCK_INSTANCE` を使用していること
- トレース出力の形式が同一ライブラリ内の既存実装と揃っていること
- 新しい mock が既存のテスト ビルド経路で参照される配置に置かれていること

## app mock の弱リンク

app の関数と同名のラッパーを提供する場合は、`testfw.h` の `MOCK_WEAK_IMPL` を使用します。  
関数へ `WEAK_ATR` を直接付けません。

`MOCK_WEAK_IMPL` は、Linux の弱定義と Windows の `/ALTERNATENAME` に必要な実装シンボルを生成します。  
MSVC でそのオブジェクトが静的ライブラリから取り込まれない場合は、mock の公開ヘッダーに `/INCLUDE:_mock_impl_<func>` を追加します。  
real delegate が同じオブジェクトを直接参照する構成では、追加の `/INCLUDE` は不要です。

ライブラリ固有の保持用マクロがある場合は、そのマクロを公開ヘッダーへ集約します。  
同じ pragma を各関数の `.cc` へ重複して記載しません。

## 既存のテストへ影響する変更

新しい関数を置換対象へ追加すると、既存のテストが影響を受ける場合があります。追加後は全体のビルドとテストで確認してください。

### mock_libc をリンクしていないテストが未解決シンボルで落ちる

`include_override` は `mock_libc` のリンク有無に関係なく、`TEST_SRCS` に指定したすべてのソースへ適用されます。  
そのため、新しく置換対象にした関数を呼ぶソースを持つテストは、`mock_<lib>` をリンクしていないと `mock_<func>` が未解決になります。

追加前に、対象関数を呼ぶソースを持ち、かつ `mock_<lib>` をリンクしていないテストを洗い出してください。該当したテストには `LIBS += mock_<lib>` と、必要になった理由をコメントで追加します。

### 新設した override ヘッダーは既存の依存ファイルに載らない

`.d` ファイルには、その時点で実際に取り込まれたヘッダーのパスが記録されます。  
`include_override/` へ新しいヘッダーを追加しても、既存の `.d` には本物のヘッダー (`/usr/include/...` など) しか載っていないため、対象のソースは再コンパイルされません。

この状態ではテストは成功しますが、置換が効かないまま古いオブジェクトが使われ、狙った分岐に到達しません。override ヘッダーを新設したときは、影響範囲の `obj/` を削除してから再ビルドしてください。

> [!NOTE]
> カバレッジが上がらない、`EXPECT_CALL` が「呼び出されなかった」で失敗する、という形で現れます。
> モックの実装を疑う前に、対象のオブジェクトが再コンパイルされているかを確認してください。

### 複数の override ヘッダーで 1 つのモック ヘッダーを共有する場合

`openssl/evp.h` と `openssl/rand.h` のように、複数の override ヘッダーが同じモック ヘッダーを参照する構成では、`_IN_OVERRIDE_HEADER_*` フラグの扱いに注意が必要です。

モック ヘッダーの先頭で本物のヘッダーを無条件に取り込むと、もう一方の override ヘッダーが同じフラグを `#undef` して、置換マクロが定義されないまま処理が進みます。

```c
/* override 経由の場合、本物のヘッダーは include_override 側で取り込み済みである */
#ifndef _IN_OVERRIDE_HEADER_OPENSSL_H
    #include <openssl/evp.h>
    #include <openssl/rand.h>
#endif
```

> [!NOTE]
> 置換が無効になると、C のソースをコンパイルしているのに Mock クラス側の分岐が選ばれ、`algorithm が見つかりません` のような C++ 標準ライブラリのエラーになります。
> 症状から原因が読み取りにくいため、この構成を採るときは最初からガードを入れてください。

## 参照例

- Mock クラス
    - `../include/mock_stdio.h`
    - `../include/mock_unistd.h`
- override ヘッダー
    - `../include_override/stdio.h`
    - `../include_override/unistd.h`
    - `../include_override/io.h` (Windows の `_close` / `_read` など。`mock_unistd.h` と共用)
- 関数実装
    - `../libsrc/mock_libc/`
