# How to test

## 単体テストのソース ファイルを指定する方法

単体テストの `makepart.mk` では、テストで動作を確認し、カバレッジを充足する対象のソース ファイルだけを `TEST_SRCS` に指定する。テスト対象のリンクを成立させる依存実装やテスト固有の補助実装など、カバレッジ対象にしないソース ファイルは `ADD_SRCS` に指定する。

`TEST_SRCS` はテスト用コンパイル フラグと override ヘッダーによる関数置換の対象になるが、`ADD_SRCS` はこれらの対象にならない。ファイルが `prod/` 配下にあるかどうかではなく、そのテストでの役割に基づいて選択する。

詳細は [makepart.mk / makechild.mk / makelocal.mk の役割と使い分け](../../makefw/docs/makeparts.md#test_srcs-と-add_srcs-の留意事項) を参照すること。

## 個別のテストを行う方法

主にデバッグ時に、個別のテストを行う方法について説明する。

### テスト対象フォルダーにて make test

テスト対象フォルダーにて `make test` すれば、コンパイルとテストが実行される。

### make test にフィルター文字列を指定

`make test` を呼び出す際にフィルター文字列を指定し、任意のテストのみ行うことができる。

参考: [google test のオプションを使ってみた](https://country-programmer.dfkp.info/2021/05/re_google_test_003/)

```bash
make test GTEST_FILTER=*call_times_check_with_args*
```

あるいは、`GTEST_FILTER` 環境変数にフィルター文字列を指定する。

```bash
export GTEST_FILTER=*call_times_check_with_args*
make test
...
export -n GTEST_FILTER # フィルター不要になったら、GTEST_FILTER 環境変数を削除
```
