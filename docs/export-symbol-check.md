# DLL/SO のエクスポート シンボル検証

`framework/testfw/include/export_check.h` は、公開ヘッダーで宣言した関数・変数が実際に DLL (Windows) / SO (Linux) からエクスポートされているかを検証するための共通処理をまとめたヘッダーです。`#include <testfw.h>` を記述するだけで、どの app のテストからも利用できます。

testfw は特定の app の内容に依存しないフレームワークであり、以下は app 側での使い方の説明にとどめます。具体的な適用例は各 app 側のテストと、そのライブラリのドキュメントを参照してください。

## 目的

`__declspec(dllexport)` (Windows) や `visibility("default")` (Linux、共有ビルドの公開印) の付け忘れは、DLL/SO を実際に外部から利用する段階まで発覚しないことが多くあります。公開ヘッダーの宣言と、ビルド成果物の実際のエクスポート一覧を機械的に突き合わせることで、次を検出します。

- テーブルに登録したにもかかわらず export マクロの付与を忘れた場合 (実際にはエクスポートされていない)
- export マクロを付与したもののテーブルへの登録を忘れた場合、または内部シンボルが動的シンボル表に漏れた場合 (想定外のエクスポートとして検出。Windows / Linux 共通)
- 変数宣言に export マクロの付与を忘れた場合 (公開ヘッダーの静的走査で検出。テーブル登録の有無にかかわらず検出可能)

Linux の `nm -D --defined-only` が返すリンカー合成シンボル (`__bss_start` / `_edata` / `_end`) は、検査対象から除外します。

## 提供する関数・マクロ

| 名前 | 役割 |
| --- | --- |
| `TESTFW_EXPORT_STATIC_ASSERT_ENTRY(name, sig)` | EXPORT_ENTRY マクロ テーブルから `static_assert` を生成する定型マクロ |
| `TESTFW_EXPORT_NAME_ENTRY(name, sig)` | EXPORT_ENTRY マクロ テーブルから期待シンボル名配列の要素を生成する定型マクロ |
| `TESTFW_EXPORT_SIGNATURE_ENTRY(name, sig)` | EXPORT_ENTRY マクロ テーブルから「名前 → シグネチャ文字列」の map 要素を生成する定型マクロ |
| `testing::getActualExportNames(dll_or_so_path)` | 実際の DLL/SO からエクスポート シンボル名一覧を取得する (`dumpbin`/`nm` をプラットフォームに応じて内部で実行する) |
| `testing::expectExportNamesMatch(expected, actual, signatures = {})` | 期待値と実際値を突き合わせ、不足と想定外の両方を Windows / Linux 共通で `EXPECT_TRUE` で報告する (完全一致。Linux のリンカー合成シンボルは除外。`signatures` を渡すと stdout の各シンボルにシグネチャを併記) |
| `testing::findUndecoratedExternVariables(include_dir, export_macro_name)` | 指定ディレクトリ配下のヘッダーから、export マクロを伴わない `extern` 変数宣言を検出する |
| `testing::identManifestSymbolName(target)` | IDENT 機能 ([ident.md](../../makefw/docs/ident.md)) が自動生成するシンボル名を組み立てる |
| `testing::joinNames(names)` | 文字列一覧をカンマ区切りで連結する (失敗メッセージ整形用) |

## app 側で書くもの (固有定義)

app 側のテストには、EXPORT_ENTRY マクロ テーブル本体と、パスを渡すだけの `TEST_F` のみを記述します。

```cpp
#define MYLIB_EXPORT_TABLE(EXPORT_ENTRY) \
    EXPORT_ENTRY(mylib_open, int(MYLIB_API *)(void)) \
    /* ... */

MYLIB_EXPORT_TABLE(TESTFW_EXPORT_STATIC_ASSERT_ENTRY)
static const char *const kExpectedExportNames[] = {MYLIB_EXPORT_TABLE(TESTFW_EXPORT_NAME_ENTRY)};
static const std::map<std::string, std::string> kExpectedExportSignatures = {
    MYLIB_EXPORT_TABLE(TESTFW_EXPORT_SIGNATURE_ENTRY)};

TEST_F(myLibExportTest, symbol_names_match)
{
    std::set<std::string> expected(std::begin(kExpectedExportNames), std::end(kExpectedExportNames));
#if defined(PLATFORM_WINDOWS)
    expected.insert(testing::identManifestSymbolName("libmylib" TESTFW_SHARED_LIBRARY_EXTENSION));
#endif /* PLATFORM_WINDOWS */
    std::set<std::string> actual = testing::getActualExportNames(dll_path);
    testing::expectExportNamesMatch(expected, actual, kExpectedExportSignatures);
}

TEST_F(myLibExportTest, public_header_variables_declare_export_macro)
{
    std::vector<std::string> undecorated =
        testing::findUndecoratedExternVariables(include_dir, "MYLIB_EXPORT");
    EXPECT_TRUE(undecorated.empty()) << testing::joinNames(undecorated);
}
```

実シンボルを取得する処理と Linux のリンカー合成シンボルを除外する処理は、共通関数の内部で切り替わります。

Windows の共有ライブラリに自動追加される IDENT manifest や、ライブラリ固有の OS 別 API は、app 側でプラットフォームごとの期待値へ明示的に追加します。  
テスト本体で `PLATFORM_WINDOWS` または `PLATFORM_LINUX` を判定する場合は、テストの `makepart.mk` で make の同名変数に対応する `DEFINES` を追加します。  
期待値へ含めず実際値から除外すると、同じ接頭辞を持たない内部シンボルの漏出を検出できないため、接頭辞による絞り込みは行いません。

## 公開ヘッダーと DLL/SO は 1:1 とは限らない

1 つの app の `prod/include/` 全体を、1 つの DLL/SO の期待エクスポート一覧とみなしてはいけません。次のような構成があり得ます。

- 共有ライブラリとしてエクスポートするサブツリーと、静的リンク専用ライブラリ (DLL/SO を生成しない) のサブツリーが、同じ `prod/include/` 配下に混在する構成。静的リンク専用のサブツリーは export マクロを一切伴わない `extern` 宣言のみで構成されるのが正しい設計であり、`symbol_names_match` に相当するテストも `findUndecoratedExternVariables` による走査も対象外とします。
- `prod/include/` 配下の複数サブツリーが、それぞれ別々の DLL/SO に対応する構成。

したがって、app 側は「app 全体」ではなく「エクスポート単位 (ヘッダー サブディレクトリ + export マクロ名 + 対応する DLL/SO パス)」ごとに `TEST_F` を記述します。1 app に複数の DLL/SO があれば複数の `TEST_F` を用意し、静的リンク専用のサブツリーにはテストを記述しません。`include_dir` には、対応する DLL/SO のソースとなるヘッダー サブディレクトリのみを渡し、app の `prod/include/` 全体は渡さないでください。
