# AGENTS.md

## リポジトリ概要

既存の C コードや .NET プロジェクトをテストするための共通部品をまとめたテスト フレームワークです。実行補助スクリプト、モック用ヘッダー、モック実装、配布ライブラリを含みます。

## 参照先

作業に関係する文書の該当節を参照してください。

- [README.md](README.md)
- [文書一覧](docs/README.md)

## 作業時の入口

- `makefile` - ルートの入口。`libsrc/` 配下のビルドを実行
- `bin_internal/` - C/C++ テスト、.NET テスト、集計、色付け、カバレッジ変換のスクリプト群
- `bin_test/` - `bin_internal/` のスクリプトのテスト
- `include/` - テスト支援ヘッダー
- `include_override/` - 既存コードに差し替えるための override 用ヘッダー
- `libsrc/` - 共通ライブラリとモックのソース
- `lib/` - Linux / Windows 向けの生成済みライブラリ
- `gtest/` - GoogleTest の配布物を管理する独立した git ルート

## 主要コマンド

```bash
make
make clean
make test
```

## 注意点

- `lib/` には Linux と Windows の配布済み成果物があります。命名規則や配置を変更する場合は、スクリプト、README、CI をまとめて確認してください。
- `gtest/` は独立した Git リポジトリで、コミット単位を分けます。GoogleTest 配布物を変更する場合は、本書に `gtest/AGENTS.md` を重ねて適用してください。
- `TEST_SRCS` / `ADD_SRCS` で取り込まれたソースは、ビルド ディレクトリのリンクやコピーではなく、指定元の実体ファイルを変更してください。  
  詳細は [makeparts](../makefw/docs/makeparts.md) の「TEST_SRCS / ADD_SRCS の留意事項」を参照してください。
- テスト コードのフェーズ分割コメント (`// Arrange` `// Pre-Assert` `// Act` `// Assert` `// Cleanup` と `[状態]` などのブラケット タグ) は `docs/about-test-phase.md` の規則に従ってください。1 テスト内で Arrange/Act/Assert のサイクルを複数回含むマルチ フェーズ テストの番号付与規則は同ドキュメントの「シングル フェーズ テストとマルチ フェーズ テスト」を参照してください。
- 確認タグの回数式や件数集計を変更する場合は、`docs/about-test-phase.md` の「期待を確認する行為 1 回を 1 件として集計する」を参照し、`bin_internal/` の共通集計処理、C/C++・.NET の実行スクリプト、`bin_test/` の局所テストを同時に確認してください。
- 共通関数や fixture のエビデンスを変更する場合は、`docs/test-subprocedures.md` を参照してください。サブ手順の解析を変更する場合は、概要、ソース抜粋、C/C++・.NET の実行経路、コメント検査を同時に確認してください。
