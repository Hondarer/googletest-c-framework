# 構造体の宣言について

以下のコードは、g++ ではコンパイル可能だが、gcc ではコンパイルできません。

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

1. コンパイラを g++ にします。
2. IntelliSense を c++ にします。

## コンパイラを g++ にする

makefile にて makesrc.mk を include する前に以下を記載します。

```text
CC=g++
```

## IntelliSense を c++ にする

ワークスペースの settings.json に以下記載します。

```json
"files.associations": {
    "*.c": "cpp"
}
```

**特定フォルダーに適用したい場合**

** はサブディレクトリも含めて .c ファイルを検索するワイルドカード。

files.associations の設定は絶対パスのパターン マッチであり、${workspaceFolder} が利用できません。そのため、先頭に **/ を付与する必要がある。(see [issue](https://github.com/microsoft/vscode/issues/12805))

以下により、samplesubdir 配下の *.c を c++ として解釈できます。

```json
"files.associations": {
    "**/test/src/samplesubdir/**/*.c": "cpp"
}
```
