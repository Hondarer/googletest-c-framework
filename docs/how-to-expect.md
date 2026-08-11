# How to expect

This content was created by editing and refining material generated using AI.

以下はよく利用されるシナリオに関して、平易に解説したものです。  
より詳細は [GoogleTest User’s Guide](https://google.github.io/googletest/) を参照してください。

## 引数のマッチング

### char* のマッチ

Google Mock では、引数が文字列 (`char*` など) の場合、文字列比較用のマッチャーを利用できます。たとえば以下のようなマッチャーがよく使われます。

- `StrEq("期待する文字列")`: 完全一致
- `StrCaseEq("期待する文字列")`: 大文字・小文字を区別しないで一致

```cpp
// Mock 対象のインターフェース (例)
class MyInterface {
public:
    virtual ~MyInterface() {}
    virtual int DoSomething(const char* str) = 0;
};

// Mock クラス
class MockMyInterface : public MyInterface {
public:
    MOCK_METHOD(int, DoSomething, (const char* str), (override));
};

TEST(MyTest, CharPointerMatch) {
    MockMyInterface mock;

    // "Hello" という文字列が引数として渡される呼び出しを期待
    EXPECT_CALL(mock, DoSomething(StrEq("Hello")))
        .Times(1);

    // 実際のコード側: 呼び出し
    mock.DoSomething("Hello");
}
```

この例では、`DoSomething("Hello")` が 1 回呼ばれることをチェックしています。もし大文字・小文字を区別しないでチェックしたい場合は `StrCaseEq("hello")` などを利用できます。

また、引数が正規表現にマッチするかどうかをチェックしたいときは、Google Mock の `MatchesRegex` マッチャーを使用できます。

```cpp
// "test" を含む文字列にマッチさせる例
EXPECT_CALL(mock, DoSomething(MatchesRegex(".*test.*")))
    .Times(1)
    .WillOnce(Return(1));
```

この例は、「どこかに `test` という文字列が含まれていれば OK」というマッチになります。たとえば `"mytestdata"` や `"this is a test string"` などがヒットします。

## 呼び出し回数の制限 (Times)

### Times (0)

**Times(0)** は、「その関数は呼ばれてはいけない」ことを示します。

```cpp
EXPECT_CALL(mock, DoSomething(_))
    .Times(0);
```

ここで `(_)` は「どんな引数でも」というワイルドカードです。`Times(0)` と宣言しているので、テスト中に `mock.DoSomething(...)` が一度でも呼ばれるとテストが失敗します。呼ばれてほしくないパスを明示することで、誤って呼んでいないかを検証できます。

### 2 回以上の制限

呼び出し回数に制限を設ける場合、以下のように書きます。

- `Times(2)`: ちょうど 2 回呼ばれることを期待
- `Times(AtLeast(n))`: **最低** n 回以上呼ばれることを期待
- `Times(AtMost(n))`: **最大** n 回まで呼ばれることを期待
- `Times(Between(a, b))`: a 回以上、b 回以下の呼び出しを期待
- `Times(AnyNumber())`: 何回でもよい

例:

```cpp
// ちょうど 2 回呼び出される
EXPECT_CALL(mock, DoSomething(StrEq("CountUp")))
    .Times(2);

// 2 回以上なら OK
EXPECT_CALL(mock, DoSomething(StrEq("CountUp")))
    .Times(AtLeast(2));
```

## 結果 (返却値) の切り替え (WillOnce / WillRepeatedly)

### 1 回目、2 回目、3 回目以降で返却値を変える

`WillOnce` は「その指定した呼び出し回数分だけ、ある動作を行う」ために使います。複数回 `WillOnce` をつないで書くことで、回数に応じて返却値 (あるいは動作) を変化させられます。その後、さらに呼び出しが続くときは `WillRepeatedly` が使われます。

```cpp
// 例: 1 回目は 10 を返す、2 回目は 20 を返す、3 回目以降は常に -1 を返す
EXPECT_CALL(mock, DoSomething(_))
    .Times(AnyNumber())                  // 呼び出し回数自体は制限しない
    .WillOnce(Return(10))               // 1 回目
    .WillOnce(Return(20))               // 2 回目
    .WillRepeatedly(Return(-1));        // 3 回目以降
```

以下のような呼び出しを想定します。

```cpp
// 実際のコード側:
std::cout << "1回目: " << mock.DoSomething("test1") << std::endl;  // => 10
std::cout << "2回目: " << mock.DoSomething("test2") << std::endl;  // => 20
std::cout << "3回目: " << mock.DoSomething("test3") << std::endl;  // => -1
std::cout << "4回目: " << mock.DoSomething("test4") << std::endl;  // => -1
```

### 返却値以外の動作

`WillOnce(...)` / `WillRepeatedly(...)` には、`Return()` だけでなく `Invoke()` や `InvokeWithoutArgs()` など、さらに複雑な処理を実装することも可能です。たとえば「モック内でカウンターを増やしてログを出力する」などの挙動を仕込むこともできます。

## デフォルト動作の定義 (ON_CALL)

すべてのテストで `EXPECT_CALL` を書くとコードが煩雑になる場合や、「あるメソッドの既定の返却値はテスト全体で共通にしておきたい」というケースがあります。そのようなときに `ON_CALL` を使うと、モックに対する **デフォルトの動作** を定義できます。

`ON_CALL(mock, メソッド(引数マッチャ))...` は、具体的な **期待** (Expect) ではなく、「もし呼ばれたらこう振る舞う」という **動作設定** を与えます。呼び出しが本当に起こるかどうかは `EXPECT_CALL` で別に検証します。

```cpp
class MockMyInterface : public MyInterface {
public:
    MOCK_METHOD(int, DoSomething, (const char* str), (override));
};

TEST(MyTest, OnCallDefaultBehavior) {
    MockMyInterface mock;

    // デフォルト動作を設定: どんな文字列がきても -1 を返す
    ON_CALL(mock, DoSomething(_))
        .WillByDefault(Return(-1));

    // 特定のケースだけ、挙動を上書きして期待も設定
    EXPECT_CALL(mock, DoSomething(StrEq("special")))
        .WillOnce(Return(999));

    // テスト コード内の呼び出し
    // special 以外ならデフォルトの -1、"special" なら 999 を返す
    EXPECT_EQ(mock.DoSomething("foo"), -1);
    EXPECT_EQ(mock.DoSomething("special"), 999);
}
```

このように `ON_CALL` でデフォルト (既定) の動作を定義しておき、**特定の引数に対する期待と動作を `EXPECT_CALL` で上書きする**、という使い分けを行うのが適切です。

## EXPECT_CALL の引数評価順 (マッチの優先度)

Google Mock では、1 つのモック メソッドに対して複数の `EXPECT_CALL` を書くとき、**「最後に定義した期待が最初に評価される」** というルールがあります。つまり、

1. テスト コード上で書かれた順に上から下へ `EXPECT_CALL` を定義していく
2. 実行時にモック メソッドが呼び出されたら、**下 (最後) に定義した `EXPECT_CALL`** から順に「この呼び出しにマッチするか?」をチェックする
3. マッチした時点で、その `EXPECT_CALL` の動作が適用される

したがって、**特定の引数パターンを先に書き、その後に「どんな引数でもマッチする (`_`)」というパターンを書いてしまうと、後ろの `_` が優先されてしまう** 可能性があります。一般的に「もっとも限定的 (具体的) なマッチ」を後に書きたい場合は、その意図に沿って定義順を工夫する必要があります。

### 「すべてにマッチする _ は先に (上のほうで) 定義しておく」例

もし「ある条件に厳密にマッチさせたい `EXPECT_CALL`」と「デフォルトでマッチしてほしい `_`」を共存させる場合、Google Mock の“最後に定義したもの優先”の特性を踏まえると、以下のように記載するのが一般的です。

```cpp
// (1) まず「どんな引数でもマッチする」ルールを定義
EXPECT_CALL(mock, DoSomething(_))
    .Times(AnyNumber())
    .WillRepeatedly(Return(-1));

// (2) その後で、特定の引数のルールを定義 (こちらが優先される)
EXPECT_CALL(mock, DoSomething(StrEq("Hello")))
    .Times(1)
    .WillOnce(Return(999));
```

この順番にすることで、以下のような動きになります。

1. `DoSomething("Hello")` が呼ばれた → まず後で定義した (2) の `EXPECT_CALL` にマッチするかをチェック → マッチするので、**999** が返る。
2. `DoSomething("Other")` が呼ばれた → (2) はマッチしないので、次に (1) の `_` がマッチ → **-1** が返る。

もしこれを逆に書いてしまうと、(2) のほうが先に定義され、(1) のほうが後に定義されるため、`DoSomething("Hello")` が呼ばれたとき、(1) の `_` が優先して評価されてしまう (マッチしてしまう) ので、意図どおりに「Hello だけ特別な返却値」を返せなくなる可能性が高いです。

## EXPECT_CALL で呼び出し順序をテストする

Google Mock では、複数のメソッド呼び出しが特定の順序で行われることをテストできます。主に以下の方法があります。

### InSequence ブロックを使用する

`InSequence` オブジェクトをスコープ内に配置すると、そのスコープ内で記述した `EXPECT_CALL` が記述順に呼ばれることを期待します。

```cpp
{
    testing::InSequence seq;

    EXPECT_CALL(mock, MethodA()).Times(1);
    EXPECT_CALL(mock, MethodB()).Times(1);
}

// ここでは、MethodA() → MethodB() の順で呼び出されることをテスト
// 実際に MethodB() → MethodA() の順で呼ばれるとテスト失敗になる
```

`InSequence` はシンプルで扱いやすい方法です。複数のメソッドに対して呼び出し順序をテストしたい場合に便利です。

### Sequence オブジェクトを使う

より複雑な順序制御を行う場合は、`testing::Sequence` オブジェクトを使います。`Sequence` を使うと、複数のシーケンスを定義し、一部のメソッドはシーケンス 1 の順序に従う、一部のメソッドはシーケンス 2 の順序に従う、といった分割もできます。

```cpp
Sequence s1, s2;

// s1 に属する呼び出し: M1 → M2 の順で呼ばれることを期待
EXPECT_CALL(mock, M1())
    .InSequence(s1);
EXPECT_CALL(mock, M2())
    .InSequence(s1);

// s2 に属する呼び出し: M3 → M4 の順で呼ばれることを期待
EXPECT_CALL(mock, M3())
    .InSequence(s2);
EXPECT_CALL(mock, M4())
    .InSequence(s2);

// これら 2 つのシーケンス (s1, s2) はそれぞれ独立して順序をチェックする
// M1 → M2 の順と、M3 → M4 の順が守られていれば OK
```

シーケンス間の順序制御はとくに指定しない限り独立なので、たとえば s1 に属する呼び出しの途中に s2 の呼び出しが行われても構いません。

## NiceMock とは

テストを作成しているときに、「モック クラスで定義しているメソッドが未使用の場合や、意図しない呼び出しが行われる場合」の警告が煩わしく感じることがあります。Google Mock では以下のような仕組みが用意されています。

- `StrictMock<T>`: 未設定の呼び出しはすべてテスト失敗とする
- `NiceMock<T>`: 未設定の呼び出しは許容し、テストを失敗させない (警告も出さない)
- `NaggyMock<T>`: 未設定の呼び出しに対し警告を出すが、テストは失敗させない

テストを記載していく中で、「想定外の呼び出しがあったときにテストを失敗させる」という Strict な設定はテスト記述の冗長性を高めたりハードルを上げたりすることがあります。そこで、定義されていない呼び出しを許容する `NiceMock` を使うと、**明示的に `EXPECT_CALL` しなかったメソッドが呼ばれてもテスト失敗にならない** ため、コストを下げつつテストが書けるというメリットがあります。

ただし `NiceMock` は「想定外の呼び出し」があっても見逃してしまうため、大規模なテストでバグが混入しやすくなる可能性があります。最初は `NiceMock` を使いつつ、テストがこなれてきたら `NaggyMock` や `StrictMock` に切り替えていく、という運用が適切です。

### NiceMock の使用例

```cpp
using ::testing::NiceMock;

class MockMyInterface : public MyInterface {
public:
    MOCK_METHOD(int, DoSomething, (const char* str), (override));
};

TEST(MyTest, NiceMockExample) {
    // NiceMock 化
    NiceMock<MockMyInterface> mock;

    // ここでは特定の引数 "Hello" に対する期待を設定
    EXPECT_CALL(mock, DoSomething(StrEq("Hello")))
        .WillOnce(Return(123));

    // "Hello" 以外の文字列を渡してもテストが失敗したり警告が出たりしない
    mock.DoSomething("Hello");    // => 123
    mock.DoSomething("Other");    // => 期待を設定していないがエラーにならない
}
```

このように、`NiceMock` を使うと「厳密にどのメソッドが呼ばれたかはあまり気にしない」状況で、とりあえずテストを通す・試験的にモックを利用してみる、といったケースで重宝します。

## まとめ

- **引数のマッチング** では、`StrEq` や `MatchesRegex` などを使うことで、文字列や正規表現に対する一致を指定できる。  
- **呼び出し回数の制限 (Times)** では、`Times(0)` で「呼ばれてはいけない」というテスト、`Times(2)` で「ちょうど 2 回」、`AtLeast(2)` で「2 回以上」など、細かい条件を指定できる。  
- **返却値の切り替え (WillOnce, WillRepeatedly)** を用いると、呼び出し回数に応じて返却する値や動作を変化させることができる。  
- **デフォルト動作 (ON_CALL)** では、モックが呼ばれた際の標準的な挙動を定義しておき、一部ケースで `EXPECT_CALL` によって上書き (あるいは厳密な期待設定) をする使い方が一般的である。  
- **EXPECT_CALL のマッチ優先度** は「**最後に定義したものが最初にマッチ判定される**」ため、特定の引数に厳密にマッチさせたい `EXPECT_CALL` を後に書き、どんな引数でもよい `_` のようなルールは先に書くのがセオリーである。
- **呼び出し順序のテスト** は `InSequence` や `Sequence` を活用することで、「メソッド A→メソッド B の順で呼び出されること」を厳密にチェックできる。
- **NiceMock** は、未設定の呼び出しがあってもテストを失敗させず、コストを下げるために有用である。大規模・本格的にモックを利用する際は `NaggyMock` や `StrictMock` も検討するとよい。

これらを踏まえると、Google Mock で柔軟かつ意図通りのテストを組むことが可能になります。
