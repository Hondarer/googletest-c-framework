# How to mock

mock 関数を追加するときの共通ルールを示します。

## 用語

- mock
    - ここではテスト ダブル全般を指す総称として扱います。
- override ヘッダー
    - 本物のヘッダーを読み込んだ後に、関数呼び出しを mock へ差し替えるためのヘッダーです。
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

命名は既存ライブラリと揃えます。新規命名を持ち込まず、同一ライブラリ内の既存パターンに合わせてください。

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

```cpp
TEST_F(MyTest, example)
{
    Mock_sample mock;

    EXPECT_CALL(mock, sample_func(_, _)).Times(1);

    /* テスト対象コード */
}
```

この構造により、テスト コードは mock の注入と解放を明示的に管理できます。

## 実装時の共通確認項目

- 置換対象の関数と `MOCK_METHOD` のシグネチャが一致していること
- `ON_CALL` の既定動作が追加されていること
- `_mock_<lib>` の設定と解除がコンストラクター / デストラクターで行われていること
- トレース出力の形式が同一ライブラリ内の既存実装と揃っていること
- 新しい mock が既存のテスト ビルド経路で参照される配置に置かれていること

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
