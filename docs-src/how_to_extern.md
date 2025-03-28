# C言語における `extern` と実体の理解

## `extern` とは何か？

`extern` は **外部に定義された関数や変数の参照を宣言するキーワード** である。  
主に複数ファイルにまたがって関数や変数を共有する際に使用される。

## 関数と変数における `extern` の違いと使い方

### 関数の `extern`

- C 言語では、関数は **暗黙的に `extern` 扱い** になる。
- ただし、**可読性や意図の明示の観点から `extern` を明示することが推奨される**。

```c
// sample.h
extern void print_message(void); // extern を明示すると意図が伝わる
```

```c
// sample.c
#include "sample.h"
void print_message(void) {
    printf("Hello, World!\n");
}
```

```plantuml
@startuml 関数の extern による参照関係
  caption 関数の extern による参照関係
  file "sample.h" as H
  note right
  　extern void print_message();
  end note

  file "sample.c" as C
  note right
  　void print_message() {
  　　// 定義（実体）
  　}
  end note

  file "main.c" as M
  note right
  　#include "sample.h"
  　int main() {
  　　print_message();
  　}
  end note

  M --> H : 関数の宣言を参照
  C --> H : 宣言と定義を一致
  M --> C : リンク時に解決
@enduml
```

### 変数の `extern`

- **変数は `extern` を明示しなければ外部から参照できない**。
- 実体 (メモリの確保) は **1 か所のソースファイル** にだけ必要である。

```c
// global.h
extern int global_counter;
```

```c
// global.c
int global_counter = 0;
```

```plantuml
@startuml 変数の宣言と定義の関係
  caption 変数の宣言と定義の関係
  file "global.h" as H
  note right
  　extern int global_counter;
  end note

  file "global.c" as G
  note right
  　int global_counter = 0; // 実体
  end note

  file "main.c" as M
  note right
  　#include "global.h"
  　int main() {
  　　return global_counter;
  　}
  end note

  M --> H : 変数の宣言を参照
  G --> H : 宣言と定義を一致
  M --> G : リンク時に解決
@enduml
```

## C++ との相互運用 (`extern "C"`)

C++ では関数名に名前修飾が行われるため、C 関数を直接リンクできない。

### 解決策: `extern "C"` を使う

```cpp
// sample.h
#ifdef __cplusplus
extern "C" {
#endif

extern void my_function(void);

#ifdef __cplusplus
}
#endif
```

```plantuml
@startuml C++ と C 間のリンクのイメージ
  caption C++ と C 間のリンクのイメージ
  package "C++側" {
    file "main.cpp" as cpp
    note right
    　extern "C" で宣言
    end note
  }

  package "C側" {
    file "my_function.c" as c
    note right
    　C の方式で関数を定義
    end note
  }

  cpp --> c : 名前修飾なしでリンク可能
@enduml
```

## なぜヘッダファイルに実体を定義してはいけないのか?

### 理由: **複数の実体が生成される**

```c
// NG: header.h
int counter = 0; // 実体を定義してしまっている
```

これを複数ファイルで include すると、リンク時にエラーが発生する:

```
multiple definition of `counter`
```

```plantuml
@startuml ヘッダに実体があるとどうなるか (NG 例)
  caption ヘッダに実体があるとどうなるか (NG 例)
  file "header.h" as H
  note right
  　int counter = 0;
  end note

  file "file1.c" as F1
  note right
  　#include "header.h"
  end note

  file "file2.c" as F2
  note right
  　#include "header.h"
  end note

  F1 --> H
  F2 --> H

  note bottom of F1
    counter の実体が生成される
  end note

  note bottom of F2
    もう一つの counter が生成されてしまう
  end note

  H --> F1 : 実体を埋め込む
  H --> F2 : 実体を埋め込む
@enduml
```

## `extern` があっても実体がない場合は?

- コンパイルは通るが **リンク時に未定義シンボルエラー** になる。

```c
// header.h
extern int missing_var;
```

```c
// main.c
#include "header.h"
int main() {
    return missing_var; // リンクエラー
}
```

```plantuml
@startuml 宣言のみで実体がない場合
  caption 宣言のみで実体がない場合
  file "header.h" as H
  note right
  　extern int missing_var;
  end note

  file "main.c" as M
  note right
  　#include "header.h"
  　int main() { return missing_var; }
  end note

  circle "???" as X

  M --> H
  M --> X : "リンク時に missing_var の定義が見つからずエラー"
@enduml
```

## `extern` がない変数や関数はファイル内に閉じる

- **`static` を使うことで、外部に公開しないローカルなシンボルとして定義可能である**。

```c
// file.c
static int local_counter = 0; // 他ファイルからアクセス不可
```

```plantuml
@startuml static 変数と外部ファイルの関係
  caption static 変数と外部ファイルの関係
  file "file.c" as F
  note right
  　static int local_counter = 0;
  end note

  file "main.c" as M
  note right
    　ここからは local_counter は見えない
  end note

  M --> F : アクセス不可
@enduml
```

## 関数の重複

C 言語では **関数の定義は重複できない**。複数の `.o` ファイルに同じシンボル (関数名) があると、リンク時にエラーになる。

しかし、**テストやモックライブラリの構成時に例外的に許される手法がある**。

### モック関数と `#pragma weak`

テスト用に本物の関数の代わりに **モック関数を使いたい場合**、`#pragma weak` を使うことで、**モック関数を「弱い定義 (weak symbol)」として登録** し、リンク時に本物の関数があればそちらを優先させることができる。

```c
// mock.c（モック側）
#include <stdio.h>

#pragma weak real_func

void real_func() {
    printf("This is a mock!\n");
}
```

```c
// real.c（本物の実装）
#include <stdio.h>

void real_func() {
    printf("This is the real one!\n");
}
```

```plantuml
@startuml モックと実体の関数の解決関係
  caption モックと実体の関数の解決関係
  file "mock.c" as Mock
  note right
  　#pragma weak real_func
  　void real_func() {
  　　// モック実装
  　}
  end note

  file "real.c" as Real
  note right
  　void real_func() {
  　　// 本物の実装
  　}
  end note

  file "main.c" as Main
  note right
  　int main() {
  　　real_func();
  　}
  end note

  Main --> Mock : "weak symbol として参照可"
  Main --> Real : 実体があればこちらを優先
  Real ..> Mock : "weak symbol の上書き"
@enduml
```

### 実行時の振る舞い

| リンク対象                  | 実行結果                             |
|-----------------------------|--------------------------------------|
| `main.o + mock.o`           | "This is a mock!" が表示される       |
| `main.o + mock.o + real.o`  | "This is the real one!" が表示される |

### 注意点

- `#pragma weak` は **GCCなどの一部コンパイラに依存** する。
- 本番コードでは使用を避け、**主にユニットテストのための手法** として理解する。

## まとめ

| 項目                     | 説明                                       |
|--------------------------|--------------------------------------------|
| `extern`                 | 外部の定義を参照するキーワード             |
| 関数の宣言               | `extern` を省略可だが、可読性から明示推奨  |
| 実体 (定義)              | メモリを確保する唯一の場所                 |
| ヘッダでの実体定義       | 複数ファイルで include されると危険        |
| C++ 相互運用             | `extern "C"` でCの名前修飾を防止           |
| `static`                 | ファイル内限定のシンボル                   |
| 関数の重複定義           | 原則禁止だが `#pragma weak` で差し替え可能 |
| weak symbol のリンク順   | 実体が存在すればそちらが優先される         |
