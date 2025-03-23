# 構造体の宣言について

以下のコードは、g++ ではコンパイル可能だが、gcc ではコンパイルできない。

これは、構造体名とエイリアスでは、エイリアス側が正であるため。

```c++
typedef struct structa
{
    int b;
} tstructa;

void samplefunc()
{
    structa structa;
}
```

拡張子が .c で上記コードが存在する場合は以下の対応が必要。

1. コンパイラを g++ にする
2. IntelliSense を c++ にする

## コンパイラを g++ にする

Makefile にて makesrc.mk を include する前に以下を記載する。

```text
CC=g++
```

## IntelliSense を c++ にする

ワークスペースの settings.json に以下記載する。

```json
"files.associations": {
    "*.c": "cpp"
}
```

**特定フォルダに適用したい場合**

** はサブディレクトリも含めて .c ファイルを検索するワイルドカード。

files.associations の設定は絶対パスのパターンマッチであり、${workspaceFolder} が利用できない。そのため、先頭に **/ を付与する必要がある。(see [issue](https://github.com/microsoft/vscode/issues/12805))

以下により、samplesubdir 配下の *.c を c++ として解釈できる。

```json
"files.associations": {
    "**/test/src/samplesubdir/**/*.c": "cpp"
}
```
