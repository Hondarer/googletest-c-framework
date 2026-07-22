# AGENTS.md

## 重要事項

- 自動ステージング、コミット禁止。指示があるまでステージング、コミットは行わないこと。
- 思考の断片は英語でもよいが、ユーザーに気づきを与えたり報告する際は日本語を用いること。

## リポジトリ概要

既存の C コードや .NET プロジェクトをテストするための共通部品をまとめたテスト フレームワークです。実行補助スクリプト、モック用ヘッダー、モック実装、配布ライブラリを含みます。

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

- スクリプト名は `bin/` が正本です。旧 `cmnd/` 前提の説明に戻さないこと。
- `lib/` には Linux と Windows の配布済み成果物が入る。命名規則や配置を変える場合はスクリプト、README、CI をまとめて確認すること。
- `gtest/` は別 repo 扱いです。GoogleTest 配布物の変更はその直下の `AGENTS.md` と `README.md` に従うこと。
- `TEST_SRCS` / `ADD_SRCS` に指定したソース ファイルは、`make test` 時にビルド ディレクトリへシンボリック リンクまたはコピーとして取り込まれる。ビルド ディレクトリ内のファイルを直接変更しても次回 `make test` で上書きされるため、`prod/` の実体ファイルを変更すること。  
  see: `framework/makefw/docs/makeparts.md` の「TEST_SRCS / ADD_SRCS の留意事項」
- テスト コードのフェーズ分割コメント (`// Arrange` `// Pre-Assert` `// Act` `// Assert` `// Cleanup` と `[状態]` 等のブラケット タグ) は `docs/about-test-phase.md` の規則に従うこと。1 テスト内で Arrange/Act/Assert のサイクルを複数回含むマルチフェーズ テストの番号付与規則は同ドキュメントの「シングルフェーズ テストとマルチフェーズ テスト」を参照すること。
