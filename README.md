# googletest-c-framework

A common modules to test existing C programs using googletest.

いわゆる「レガシーな C コード」を googletest でテスト可能にするための知見の蓄積。

本レポジトリでは、複数のテストプロジェクトで共通的に利用される共通ファイル群を切り出している。  
実際の利用例など、より個別のユースケースに近い部分に関しては、 [googletest-c-sample](https://github.com/Hondarer/googletest-c-sample) を参照。

## フォルダ構成

- bin テスト支援コマンド類
- include モックの include
- include_override テスト対象コードをビルドする際に、注入したい差分 include
- lib モックのアーカイブ
- libsrc モックのソース

配置は以下のどちらでも動作するようにしている。

- 統合プロジェクト: `framework/testfw/`
- 単独 CI: `.workspaceRoot` 配下に `framework/testfw/` と `framework/makefw/` を作る統合配置相当

必要に応じて `TESTFW_DIR=/path/to/testfw` を make 変数で明示指定できる。

### bin 内のスクリプト

#### C/C++ テスト用

| ファイル | 説明 |
|---------|------|
| `exec_test_c_cpp.sh` | C/C++ テスト実行スクリプト |
| `get_test_code_c_cpp.awk` | テストコード抽出 |
| `insert_summary_c_cpp.awk` | テストサマリ生成 |

#### .NET テスト用

| ファイル | 説明 |
|---------|------|
| `exec_test_dotnet.sh` | .NET テスト実行スクリプト (一括実行) |
| `get_test_code_dotnet.py` | テストコード抽出 |
| `insert_summary_dotnet.py` | テストサマリ生成 |
| `parse_trx_results.py` | TRX XML パーサー |
| `extract_dotnet_output.py` | バッチ出力から個別テスト結果を抽出 |

#### 共通

| ファイル | 説明 |
|---------|------|
| `banner.sh` | テスト結果バナー表示 |

## ビルド

- 統合プロジェクト: `make -C framework/testfw clean; make -C framework/testfw`
- 単独リポジトリ CI: `cd framework/testfw && make clean && make`
- 任意配置: `make -C "$TESTFW_DIR" clean; make -C "$TESTFW_DIR"`

## 依存コンポーネント

### C/C++ テスト

- gcc
- g++
- make
- googletest (google mock)
- iconv
  ※ ソースが UTF-8 以外のエンコーディング (例: EUC-JP) の場合に、コンパイラ/リンカ出力を UTF-8 に変換するために使用。glibc に含まれるため、追加インストールは通常不要。

### .NET テスト

- .NET SDK
- python3
- xUnit

## 任意コンポーネント

- gcovr
- lcov

## ドキュメント

テストフレームワークの詳細な機能説明は、`docs/` ディレクトリにあります。
