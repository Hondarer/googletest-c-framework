# googletest-c-framework

A common modules to test existing C programs using googletest.

いわゆる「レガシーな C コード」を googletest でテスト可能にするための知見の蓄積。

本レポジトリでは、複数のテストプロジェクトで共通的に利用される共通ファイル群を切り出している。  
実際の利用例など、より個別のユースケースに近い部分に関しては、 [googletest-c-sample](https://github.com/Hondarer/googletest-c-sample) を参照。

## フォルダ構成

- cmnd テスト支援コマンド類
- include モックの include
- include_override テスト対象コードをビルドする際に、注入したい差分 include
- lib モックのアーカイブ
- libsrc モックのソース

※ 全体が プロジェクトフォルダの testfw/ に配置される前提。

## ビルド

親階層で、`make all` (= clean & make) または `make`

## 依存コンポーネント

- gcc
- g++
- make
- googletest (google mock)
- nkf  
  ※ nkf は現在ではディストリビューションに同梱されないことが多いが、ソースが EUC-JP かつ VS Code でログを utf-8 に変換して出力するために使用。

## 任意コンポーネント

- gcovr
- lcov

## ドキュメント

テストフレームワークの詳細な機能説明は、`docs-src/` ディレクトリにあります。
