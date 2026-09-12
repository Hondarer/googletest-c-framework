# 構造体の宣言について

以下のコードは、g++ ではコンパイルできますが、gcc ではコンパイルできません。

これは、構造体名とエイリアスではエイリアス側が正となるためです。

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

拡張子が `.c` で上記のコードが存在する場合は、以下の対応が必要です。

1. コンパイラを g++ に変更する
2. IntelliSense の言語モードを C++ に設定する

## コンパイラを g++ に変更する

makefile で `makesrc.mk` を include する前に以下を記述します。

```text
CC=g++
```

## IntelliSense を C++ に設定する

ワークスペースの `settings.json` に以下を記述します。

```json
"files.associations": {
    "*.c": "cpp"
}
```

### 特定フォルダーに適用する場合

`**` はサブディレクトリも含めて `.c` ファイルを再帰的に検索するワイルドカードです。

`files.associations` の設定は絶対パスのパターン マッチであり、`${workspaceFolder}` が利用できません。そのため、先頭に `**/` を付与する必要があります (see: [Issue #12805](https://github.com/microsoft/vscode/issues/12805))。

以下の設定により、`samplesubdir` 配下の `*.c` を C++ として解釈させることができます。

```json
"files.associations": {
    "**/test/src/samplesubdir/**/*.c": "cpp"
}
```
