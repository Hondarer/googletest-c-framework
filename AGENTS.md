# AGENTS.md

## リポジトリ概要

既存の C コードや .NET プロジェクトをテストするための共通部品をまとめたテスト フレームワークです。実行補助スクリプト、モック用ヘッダー、モック実装、配布ライブラリを含みます。

## 必須参照

- [README.md](README.md)
- [文書一覧](docs/README.md)

## 作業時の入口

- `makefile` - ルートの入口。`libsrc/` 配下のビルドを呼び出す
- `bin/` - C/C++ テスト、.NET テスト、集計、色付け、カバレッジ変換のスクリプト群
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

- スクリプト名は `bin/` が正本です。旧 `cmnd/` を前提とする説明へ変更しないでください。
- `lib/` には Linux と Windows の配布済み成果物があります。命名規則や配置を変更する場合は、スクリプト、README、CI をまとめて確認してください。
- `gtest/` は別リポジトリとして扱います。GoogleTest 配布物を変更する場合は、その直下の `AGENTS.md` と `README.md` に従ってください。
- `TEST_SRCS` / `ADD_SRCS` に指定したソース ファイルは、`make test` 時にビルド ディレクトリへシンボリック リンクまたはコピーとして取り込まれます。ビルド ディレクトリ内のファイルを直接変更しても次回 `make test` で上書きされるため、`prod/` の実体ファイルを変更してください。  
  see: `framework/makefw/docs/makeparts.md` の「TEST_SRCS / ADD_SRCS の留意事項」
- テスト コードのフェーズ分割コメント (`// Arrange` `// Pre-Assert` `// Act` `// Assert` `// Cleanup` と `[状態]` 等のブラケット タグ) は `docs/about-test-phase.md` の規則に従ってください。1 テスト内で Arrange/Act/Assert のサイクルを複数回含むマルチ フェーズ テストの番号付与規則は同ドキュメントの「シングル フェーズ テストとマルチ フェーズ テスト」を参照してください。
