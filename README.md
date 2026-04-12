# googletest-c-framework

既存の C コードや .NET プロジェクトをテストするための共通部品をまとめたフレームワークです。

## 概要

この repo には以下が含まれます。

- テスト実行、結果抽出、集計、色付けのスクリプト
- モック用ヘッダーと override 用ヘッダー
- 共通ライブラリやモックのソース
- Linux / Windows 向けの生成済みライブラリ
- GoogleTest 配布物を管理する `gtest/` サブモジュール

## 主なファイル

- `makefile` - ルートの入口
- `bin/` - 実行補助スクリプト
- `include/`、`include_override/` - テスト用ヘッダー
- `libsrc/` - 共通ライブラリやモックのソース
- `lib/` - 配布ライブラリ

## ビルド

```bash
make
make clean
```

## 依存関係

- gcc
- g++
- make
- Python 3
- .NET SDK
- GoogleTest / Google Mock 配布物

## 補足

- C/C++ テストと .NET テストの補助スクリプトはすべて `bin/` にあります。
- 詳細な作業ルールは [AGENTS.md](./AGENTS.md)、補足資料は `docs/` を参照してください。

## ライセンス

[LICENSE](./LICENSE) を参照してください。
