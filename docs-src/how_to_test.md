# How to test

## 個別のテストを行う方法

主にデバッグ時に、個別のテストを行う方法について説明する。

### テスト対象フォルダにて `make test`

テスト対象フォルダにて `make test` すれば、コンパイルとテストが実行される。

### `make test` にフィルター文字列を指定

`make test` を呼び出す際にフィルター文字列を指定し、任意のテストのみ行うことができる。

参考: [google testのオプションを使ってみた](https://country-programmer.dfkp.info/2021/05/re_google_test_003/)

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
