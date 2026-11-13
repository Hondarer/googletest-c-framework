# googletest-c-framework

既存の C コードや .NET プロジェクトをテストするための共通部品をまとめたフレームワークです。

## 概要

このリポジトリには次の構成要素が含まれます。

- テスト実行、結果抽出、集計、色付けのスクリプト
- モック用ヘッダーと override 用ヘッダー
- 共通ライブラリやモックのソース
- Linux / Windows 向けの生成済みライブラリ
- GoogleTest 配布物を管理する `gtest/` サブモジュール

## 主なファイル

- `makefile` - ルートの入口
- `bin_internal/` - 実行補助スクリプト
- `bin_test/` - 実行補助スクリプトのテスト
- `include/`、`include_override/` - テスト用ヘッダー
- `libsrc/` - 共通ライブラリやモックのソース
- `lib/` - 配布ライブラリ

## ビルド

```bash
make
make clean
```

## Linux ライブラリの配置

Linux の CI 表示名は Oracle Linux に合わせて `ol8`、`ol9`、`ol10` とします。

makefw が生成する `TARGET_ARCH` とライブラリの配置先は、RHEL 系 OS の互換性を示す `el8`、`el9`、`el10` を使用します。

- Oracle Linux 8: `lib/linux_el8_x64/`
- Oracle Linux 9: `lib/linux_el9_x64/`
- Oracle Linux 10: `lib/linux_el10_x64/`

## 依存関係

- GCC
- g++
- make
- Python 3
- .NET SDK
- GoogleTest / Google Mock 配布物

## 補足

- C/C++ テストと .NET テストの補助スクリプトはすべて `bin_internal/` にあります。
- 詳細な作業ルールは [AGENTS.md](./AGENTS.md)、補足資料は `docs/` を参照してください。

## ライセンス

[LICENSE](./LICENSE) を参照してください。

## 公開 API 表から呼び出し元付きの C mock 宣言を生成します

`bin_internal/generate_shared_lib_mock_overrides.py` は、関数 API 表から C リンケージの mock 宣言と、呼び出し元を注入する関数形式マクロを生成します。  
API 表の各行は、戻り値型、配布元の関数名、固有の公開関数名、実引数の型付き宣言、実引数の呼び出し式、mock 引数の型付き宣言、mock 呼び出し式、matcher の 8 列です。  
mock 引数の先頭にはファイル名、行番号、関数名を記録します。

```sh
python3 framework/testfw/bin_internal/generate_shared_lib_mock_overrides.py \
    example_api_table.h example_overrides.h --macro-prefix MOCK_EXAMPLE \
    --variadic-api-table example_variadic_api_table.h
```

可変長 API がない場合は `--variadic-api-table` を省略します。  
専用表は戻り値型、配布元名、公開名、固定引数宣言、固定引数式、`va_list` へ集約した mock 引数宣言、mock 引数式の 7 列です。  
表内の条件付きコンパイルは生成する宣言と注入マクロにも反映します。  
mock ライブラリの `pre-build` フックから生成器を呼び、API 表の更新と出力を同期してください。

テスト対象ソースの差し替えヘッダーは、実ヘッダーの宣言を取り込んだ後、`MOCK_EXAMPLE_INJECT_CALLER` を定義して生成ヘッダーを読み込みます。  
app の差し替えヘッダーは、mock をリンクするテストの明示的な有効化定義を確認して注入します。  
実ライブラリをリンクするテストは有効化せず、依存 app の探索パスだけで mock へ切り替わることを防ぎます。  
Google Mock のテスト翻訳単位は注入を有効にせず、関数名のマクロが `EXPECT_CALL` や `ON_CALL` のメソッド名へ干渉することを避けます。  
関数ポインター用の固有名ラッパーは、取得不能なファイル名と関数名に `"<unknown>"`、行番号に `0` を渡します。

Linux の名前空間分離を確認する場合は、`bin_internal/shared_namespace_probe.py` の `verify_namespace()` を app の局所テストから使用します。  
配置済みライブラリを使い、旧名を持つ代用ライブラリと実 API を呼ぶ検証用共有ライブラリを両順で `RTLD_NOW | RTLD_GLOBAL` により読み込みます。  
実 API の結果と `dladdr()` による関数の所属先を確認し、ライブラリが未配置の場合は失敗します。
