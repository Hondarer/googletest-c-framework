// モック簡易生成
//
// C 言語の関数をモックするためのクラスや関数の宣言と定義を簡易生成するためのマクロ
//

// ---------------------------------------------------------------------------

// mock 対象の関数宣言が含まれているヘッダを include
// #include "testfunc.h"

//
// 作成したいモックの宣言
//
// MOCK_C_METHOD(戻り値の型, 関数名, 引数..., デフォルトの戻り値を表すラムダ式)
// MOCK_C_METHOD(void, 関数名, 引数...)
//   ※ 戻り値の型が void の場合のみ、デフォルトの戻り値を表すラムダ式を指定しない
//   ※ Return() を用いて、戻り値を表すラムダ式を簡易的に記述可能
//

// func1 の自動宣言 (Mock_func1 クラスの宣言も自動生成される)
// MOCK_C_METHOD(int, func1, int, int, [](int a, int b)
//               { return a + b; });

// func2 の自動宣言
// MOCK_C_METHOD(int, func2, int, int, Return(-1));

// func3 の自動宣言
// MOCK_C_METHOD(void *, func3, int, int, Return((void *)NULL));

// func4 の自動宣言
// MOCK_C_METHOD(void, func4, int, int);

// ---------------------------------------------------------------------------

#include <gmock/gmock.h>

using namespace testing;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

// val を返すラムダ式
#define Return(val) [=](auto..., auto...) { return val; }

// トークン連結などのヘルパー
#define CAT(a, b) CAT_I(a, b)
#define CAT_I(a, b) a##b
#define CAT3(a, b, c) CAT(a, CAT(b, c))
#define CAT4(a, b, c, d) CAT(a, CAT3(b, c, d))

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) a##b

// Probe トリック
#define PROBE() ~, 1
#define SECOND(a, b, ...) b
#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)

// IS_VOID(x): x が void のときだけ PROBE() を呼び出し、それ以外は何も呼ばない
#define IS_VOID(x) IS_PROBE(PP_CAT(IS_VOID_, x)())
#define IS_VOID_void() PROBE()

// 引数カウント
#define VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(_, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// VOID / NONVOIDの文字列化
#define VOID VOID
#define NONVOID NONVOID

#define BOOL_TO_VOIDNONVOID(b) CAT(BOOL_TO_VOIDNONVOID_, b)
#define BOOL_TO_VOIDNONVOID_1 VOID
#define BOOL_TO_VOIDNONVOID_0 NONVOID

// 減算テーブル (引数の数の違いを吸収する)
#define DEC_0 0
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8
#define DEC_10 9

#define DEC(x) DEC_##x

// for void returns we keep the raw count; for nonvoid we subtract 1 (the default‐lambda)
#define VOID_COUNT(count) count
#define NONVOID_COUNT(count) DEC(count)

#define DISPATCH_VOID_NONVOID(isvoid, count, ...)        \
    CAT4(                                                \
        MOCK_C_METHOD,                                   \
        CAT(BOOL_TO_VOIDNONVOID(isvoid), _COUNT)(count), \
        _,                                               \
        BOOL_TO_VOIDNONVOID(isvoid))(__VA_ARGS__)

// エントリーポイント
#define MOCK_C_METHOD(ret, name, ...) \
    DISPATCH_VOID_NONVOID(IS_VOID(ret), VA_NARGS(__VA_ARGS__), ret, name, __VA_ARGS__)

// .h と .cc の内容セレクタ
#ifndef _IN_MOCK_FACTORY_SRC_

#define MOCK_C_METHOD0_NONVOID(ret, name, default_lambda) \
    MOCK_C_METHOD0_NONVOID_H(ret, name, default_lambda)
#define MOCK_C_METHOD0_VOID(ret, name, _) \
    MOCK_C_METHOD0_VOID_H(ret, name)
#define MOCK_C_METHOD1_NONVOID(ret, name, A1, default_lambda) \
    MOCK_C_METHOD1_NONVOID_H(ret, name, A1, default_lambda)
#define MOCK_C_METHOD1_VOID(ret, name, A1) \
    MOCK_C_METHOD1_VOID_H(ret, name, A1)
#define MOCK_C_METHOD2_NONVOID(ret, name, A1, A2, default_lambda) \
    MOCK_C_METHOD2_NONVOID_H(ret, name, A1, A2, default_lambda)
#define MOCK_C_METHOD2_VOID(ret, name, A1, A2) \
    MOCK_C_METHOD2_VOID_H(ret, name, A1, A2)
#define MOCK_C_METHOD3_NONVOID(ret, name, A1, A2, A3, default_lambda) \
    MOCK_C_METHOD3_NONVOID_H(ret, name, A1, A2, A3, default_lambda)
#define MOCK_C_METHOD3_VOID(ret, name, A1, A2, A3) \
    MOCK_C_METHOD3_VOID_H(ret, name, A1, A2, A3)
#define MOCK_C_METHOD4_NONVOID(ret, name, A1, A2, A3, A4, default_lambda) \
    MOCK_C_METHOD4_NONVOID_H(ret, name, A1, A2, A3, A4, default_lambda)
#define MOCK_C_METHOD4_VOID(ret, name, A1, A2, A3, A4) \
    MOCK_C_METHOD4_VOID_H(ret, name, A1, A2, A3, A4)
#define MOCK_C_METHOD5_NONVOID(ret, name, A1, A2, A3, A4, A5, default_lambda) \
    MOCK_C_METHOD5_NONVOID_H(ret, name, A1, A2, A3, A4, A5, default_lambda)
#define MOCK_C_METHOD5_VOID(ret, name, A1, A2, A3, A4, A5) \
    MOCK_C_METHOD5_VOID_H(ret, name, A1, A2, A3, A4, A5)
#define MOCK_C_METHOD6_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, default_lambda) \
    MOCK_C_METHOD6_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, default_lambda)
#define MOCK_C_METHOD6_VOID(ret, name, A1, A2, A3, A4, A5, A6) \
    MOCK_C_METHOD6_VOID_H(ret, name, A1, A2, A3, A4, A5, A6)
#define MOCK_C_METHOD7_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda) \
    MOCK_C_METHOD7_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda)
#define MOCK_C_METHOD7_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7) \
    MOCK_C_METHOD7_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7)
#define MOCK_C_METHOD8_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda) \
    MOCK_C_METHOD8_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda)
#define MOCK_C_METHOD8_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8) \
    MOCK_C_METHOD8_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8)
#define MOCK_C_METHOD9_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda) \
    MOCK_C_METHOD9_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda)
#define MOCK_C_METHOD9_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
    MOCK_C_METHOD9_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define MOCK_C_METHOD10_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda) \
    MOCK_C_METHOD10_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda)
#define MOCK_C_METHOD10_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
    MOCK_C_METHOD10_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)

#else // _IN_MOCK_FACTORY_SRC_

#define MOCK_C_METHOD0_NONVOID(ret, name, default_lambda) \
    MOCK_C_METHOD0_NONVOID_H(ret, name, default_lambda)   \
    MOCK_C_METHOD0_NONVOID_CC(ret, name, default_lambda)
#define MOCK_C_METHOD0_VOID(ret, name, _) \
    MOCK_C_METHOD0_VOID_H(ret, name)      \
    MOCK_C_METHOD0_VOID_CC(ret, name)
#define MOCK_C_METHOD1_NONVOID(ret, name, A1, default_lambda) \
    MOCK_C_METHOD1_NONVOID_H(ret, name, A1, default_lambda)   \
    MOCK_C_METHOD1_NONVOID_CC(ret, name, A1, default_lambda)
#define MOCK_C_METHOD1_VOID(ret, name, A1) \
    MOCK_C_METHOD1_VOID_H(ret, name, A1)   \
    MOCK_C_METHOD1_VOID_CC(ret, name, A1)
#define MOCK_C_METHOD2_NONVOID(ret, name, A1, A2, default_lambda) \
    MOCK_C_METHOD2_NONVOID_H(ret, name, A1, A2, default_lambda)   \
    MOCK_C_METHOD2_NONVOID_CC(ret, name, A1, A2, default_lambda)
#define MOCK_C_METHOD2_VOID(ret, name, A1, A2) \
    MOCK_C_METHOD2_VOID_H(ret, name, A1, A2)   \
    MOCK_C_METHOD2_VOID_CC(ret, name, A1, A2)
#define MOCK_C_METHOD3_NONVOID(ret, name, A1, A2, A3, default_lambda) \
    MOCK_C_METHOD3_NONVOID_H(ret, name, A1, A2, A3, default_lambda)   \
    MOCK_C_METHOD3_NONVOID_CC(ret, name, A1, A2, A3, default_lambda)
#define MOCK_C_METHOD3_VOID(ret, name, A1, A2, A3) \
    MOCK_C_METHOD3_VOID_H(ret, name, A1, A2, A3)   \
    MOCK_C_METHOD3_VOID_CC(ret, name, A1, A2, A3)
#define MOCK_C_METHOD4_NONVOID(ret, name, A1, A2, A3, A4, default_lambda) \
    MOCK_C_METHOD4_NONVOID_H(ret, name, A1, A2, A3, A4, default_lambda)   \
    MOCK_C_METHOD4_NONVOID_CC(ret, name, A1, A2, A3, A4, default_lambda)
#define MOCK_C_METHOD4_VOID(ret, name, A1, A2, A3, A4) \
    MOCK_C_METHOD4_VOID_H(ret, name, A1, A2, A3, A4)   \
    MOCK_C_METHOD4_VOID_CC(ret, name, A1, A2, A3, A4)
#define MOCK_C_METHOD5_NONVOID(ret, name, A1, A2, A3, A4, A5, default_lambda) \
    MOCK_C_METHOD5_NONVOID_H(ret, name, A1, A2, A3, A4, A5, default_lambda)   \
    MOCK_C_METHOD5_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, default_lambda)
#define MOCK_C_METHOD5_VOID(ret, name, A1, A2, A3, A4, A5) \
    MOCK_C_METHOD5_VOID_H(ret, name, A1, A2, A3, A4, A5)   \
    MOCK_C_METHOD5_VOID_CC(ret, name, A1, A2, A3, A4, A5)
#define MOCK_C_METHOD6_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, default_lambda) \
    MOCK_C_METHOD6_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, default_lambda)   \
    MOCK_C_METHOD6_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, default_lambda)
#define MOCK_C_METHOD6_VOID(ret, name, A1, A2, A3, A4, A5, A6) \
    MOCK_C_METHOD6_VOID_H(ret, name, A1, A2, A3, A4, A5, A6)   \
    MOCK_C_METHOD6_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6)
#define MOCK_C_METHOD7_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda) \
    MOCK_C_METHOD7_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda)   \
    MOCK_C_METHOD7_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda)
#define MOCK_C_METHOD7_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7) \
    MOCK_C_METHOD7_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7)   \
    MOCK_C_METHOD7_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7)
#define MOCK_C_METHOD8_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda) \
    MOCK_C_METHOD8_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda)   \
    MOCK_C_METHOD8_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda)
#define MOCK_C_METHOD8_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8) \
    MOCK_C_METHOD8_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8)   \
    MOCK_C_METHOD8_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8)
#define MOCK_C_METHOD9_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda) \
    MOCK_C_METHOD9_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda)   \
    MOCK_C_METHOD9_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda)
#define MOCK_C_METHOD9_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
    MOCK_C_METHOD9_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9)   \
    MOCK_C_METHOD9_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define MOCK_C_METHOD10_NONVOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda) \
    MOCK_C_METHOD10_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda)   \
    MOCK_C_METHOD10_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda)
#define MOCK_C_METHOD10_VOID(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
    MOCK_C_METHOD10_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)   \
    MOCK_C_METHOD10_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)

#endif // _IN_MOCK_FACTORY_SRC_

// 0 引数 非 void 戻り値
#define MOCK_C_METHOD0_NONVOID_H(ret, name, default_lambda) \
    class Mock_##name                                       \
    {                                                       \
    public:                                                 \
        MOCK_METHOD0(name, ret());                          \
        Mock_##name();                                      \
        ~Mock_##name();                                     \
    };                                                      \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD0_NONVOID_CC(ret, name, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                     \
    Mock_##name::Mock_##name()                               \
    {                                                        \
        ON_CALL(*this, name())                               \
            .WillByDefault(Invoke(default_lambda));          \
        _mock_##name = this;                                 \
    }                                                        \
    Mock_##name::~Mock_##name()                              \
    {                                                        \
        _mock_##name = nullptr;                              \
    }                                                        \
    ret name()                                               \
    {                                                        \
        return _mock_##name                                  \
                   ? _mock_##name->name()                    \
                   : default_lambda();                       \
    }

// 1 引数 非 void 戻り値
#define MOCK_C_METHOD1_NONVOID_H(ret, name, A1, default_lambda) \
    class Mock_##name                                           \
    {                                                           \
    public:                                                     \
        MOCK_METHOD1(name, ret(A1));                            \
        Mock_##name();                                          \
        ~Mock_##name();                                         \
    };                                                          \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD1_NONVOID_CC(ret, name, A1, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                         \
    Mock_##name::Mock_##name()                                   \
    {                                                            \
        ON_CALL(*this, name(_))                                  \
            .WillByDefault(Invoke(default_lambda));              \
        _mock_##name = this;                                     \
    }                                                            \
    Mock_##name::~Mock_##name()                                  \
    {                                                            \
        _mock_##name = nullptr;                                  \
    }                                                            \
    ret name(A1 _1)                                              \
    {                                                            \
        return _mock_##name                                      \
                   ? _mock_##name->name(_1)                      \
                   : default_lambda(_1);                         \
    }

// 2 引数 非 void 戻り値
#define MOCK_C_METHOD2_NONVOID_H(ret, name, A1, A2, default_lambda) \
    class Mock_##name                                               \
    {                                                               \
    public:                                                         \
        MOCK_METHOD2(name, ret(A1, A2));                            \
        Mock_##name();                                              \
        ~Mock_##name();                                             \
    };                                                              \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD2_NONVOID_CC(ret, name, A1, A2, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                             \
    Mock_##name::Mock_##name()                                       \
    {                                                                \
        ON_CALL(*this, name(_, _))                                   \
            .WillByDefault(Invoke(default_lambda));                  \
        _mock_##name = this;                                         \
    }                                                                \
    Mock_##name::~Mock_##name()                                      \
    {                                                                \
        _mock_##name = nullptr;                                      \
    }                                                                \
    ret name(A1 _1, A2 _2)                                           \
    {                                                                \
        return _mock_##name                                          \
                   ? _mock_##name->name(_1, _2)                      \
                   : default_lambda(_1, _2);                         \
    }

// 3 引数 非 void 戻り値
#define MOCK_C_METHOD3_NONVOID_H(ret, name, A1, A2, A3, default_lambda) \
    class Mock_##name                                                   \
    {                                                                   \
    public:                                                             \
        MOCK_METHOD3(name, ret(A1, A2, A3));                            \
        Mock_##name();                                                  \
        ~Mock_##name();                                                 \
    };                                                                  \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD3_NONVOID_CC(ret, name, A1, A2, A3, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                 \
    Mock_##name::Mock_##name()                                           \
    {                                                                    \
        ON_CALL(*this, name(_, _, _))                                    \
            .WillByDefault(Invoke(default_lambda));                      \
        _mock_##name = this;                                             \
    }                                                                    \
    Mock_##name::~Mock_##name()                                          \
    {                                                                    \
        _mock_##name = nullptr;                                          \
    }                                                                    \
    ret name(A1 _1, A2 _2, A3 _3)                                        \
    {                                                                    \
        return _mock_##name                                              \
                   ? _mock_##name->name(_1, _2, _3)                      \
                   : default_lambda(_1, _2, _3);                         \
    }

// 4 引数 非 void 戻り値
#define MOCK_C_METHOD4_NONVOID_H(ret, name, A1, A2, A3, A4, default_lambda) \
    class Mock_##name                                                       \
    {                                                                       \
    public:                                                                 \
        MOCK_METHOD4(name, ret(A1, A2, A3, A4));                            \
        Mock_##name();                                                      \
        ~Mock_##name();                                                     \
    };                                                                      \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD4_NONVOID_CC(ret, name, A1, A2, A3, A4, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                     \
    Mock_##name::Mock_##name()                                               \
    {                                                                        \
        ON_CALL(*this, name(_, _, _, _))                                     \
            .WillByDefault(Invoke(default_lambda));                          \
        _mock_##name = this;                                                 \
    }                                                                        \
    Mock_##name::~Mock_##name()                                              \
    {                                                                        \
        _mock_##name = nullptr;                                              \
    }                                                                        \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4)                                     \
    {                                                                        \
        return _mock_##name                                                  \
                   ? _mock_##name->name(_1, _2, _3, _4)                      \
                   : default_lambda(_1, _2, _3, _4);                         \
    }

// 5 引数 非 void 戻り値
#define MOCK_C_METHOD5_NONVOID_H(ret, name, A1, A2, A3, A4, A5, default_lambda) \
    class Mock_##name                                                           \
    {                                                                           \
    public:                                                                     \
        MOCK_METHOD5(name, ret(A1, A2, A3, A4, A5));                            \
        Mock_##name();                                                          \
        ~Mock_##name();                                                         \
    };                                                                          \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD5_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                         \
    Mock_##name::Mock_##name()                                                   \
    {                                                                            \
        ON_CALL(*this, name(_, _, _, _, _))                                      \
            .WillByDefault(Invoke(default_lambda));                              \
        _mock_##name = this;                                                     \
    }                                                                            \
    Mock_##name::~Mock_##name()                                                  \
    {                                                                            \
        _mock_##name = nullptr;                                                  \
    }                                                                            \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5)                                  \
    {                                                                            \
        return _mock_##name                                                      \
                   ? _mock_##name->name(_1, _2, _3, _4, _5)                      \
                   : default_lambda(_1, _2, _3, _4, _5);                         \
    }

// 6 引数 非 void 戻り値
#define MOCK_C_METHOD6_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, default_lambda) \
    class Mock_##name                                                               \
    {                                                                               \
    public:                                                                         \
        MOCK_METHOD6(name, ret(A1, A2, A3, A4, A5, A6));                            \
        Mock_##name();                                                              \
        ~Mock_##name();                                                             \
    };                                                                              \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD6_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                             \
    Mock_##name::Mock_##name()                                                       \
    {                                                                                \
        ON_CALL(*this, name(_, _, _, _, _, _))                                       \
            .WillByDefault(Invoke(default_lambda));                                  \
        _mock_##name = this;                                                         \
    }                                                                                \
    Mock_##name::~Mock_##name()                                                      \
    {                                                                                \
        _mock_##name = nullptr;                                                      \
    }                                                                                \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6)                               \
    {                                                                                \
        return _mock_##name                                                          \
                   ? _mock_##name->name(_1, _2, _3, _4, _5, _6)                      \
                   : default_lambda(_1, _2, _3, _4, _5, _6);                         \
    }

// 7 引数 非 void 戻り値
#define MOCK_C_METHOD7_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda) \
    class Mock_##name                                                                   \
    {                                                                                   \
    public:                                                                             \
        MOCK_METHOD7(name, ret(A1, A2, A3, A4, A5, A6, A7));                            \
        Mock_##name();                                                                  \
        ~Mock_##name();                                                                 \
    };                                                                                  \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD7_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                                 \
    Mock_##name::Mock_##name()                                                           \
    {                                                                                    \
        ON_CALL(*this, name(_, _, _, _, _, _, _))                                        \
            .WillByDefault(Invoke(default_lambda));                                      \
        _mock_##name = this;                                                             \
    }                                                                                    \
    Mock_##name::~Mock_##name()                                                          \
    {                                                                                    \
        _mock_##name = nullptr;                                                          \
    }                                                                                    \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7)                            \
    {                                                                                    \
        return _mock_##name                                                              \
                   ? _mock_##name->name(_1, _2, _3, _4, _5, _6, _7)                      \
                   : default_lambda(_1, _2, _3, _4, _5, _6, _7);                         \
    }

// 8 引数 非 void 戻り値
#define MOCK_C_METHOD8_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda) \
    class Mock_##name                                                                       \
    {                                                                                       \
    public:                                                                                 \
        MOCK_METHOD8(name, ret(A1, A2, A3, A4, A5, A6, A7, A8));                            \
        Mock_##name();                                                                      \
        ~Mock_##name();                                                                     \
    };                                                                                      \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD8_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                                     \
    Mock_##name::Mock_##name()                                                               \
    {                                                                                        \
        ON_CALL(*this, name(_, _, _, _, _, _, _, _))                                         \
            .WillByDefault(Invoke(default_lambda));                                          \
        _mock_##name = this;                                                                 \
    }                                                                                        \
    Mock_##name::~Mock_##name()                                                              \
    {                                                                                        \
        _mock_##name = nullptr;                                                              \
    }                                                                                        \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7, A8 _8)                         \
    {                                                                                        \
        return _mock_##name                                                                  \
                   ? _mock_##name->name(_1, _2, _3, _4, _5, _6, _7, _8)                      \
                   : default_lambda(_1, _2, _3, _4, _5, _6, _7, _8);                         \
    }

// 9 引数 非 void 戻り値
#define MOCK_C_METHOD9_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda) \
    class Mock_##name                                                                           \
    {                                                                                           \
    public:                                                                                     \
        MOCK_METHOD9(name, ret(A1, A2, A3, A4, A5, A6, A7, A8, A9));                            \
        Mock_##name();                                                                          \
        ~Mock_##name();                                                                         \
    };                                                                                          \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD9_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                                         \
    Mock_##name::Mock_##name()                                                                   \
    {                                                                                            \
        ON_CALL(*this, name(_, _, _, _, _, _, _, _, _))                                          \
            .WillByDefault(Invoke(default_lambda));                                              \
        _mock_##name = this;                                                                     \
    }                                                                                            \
    Mock_##name::~Mock_##name()                                                                  \
    {                                                                                            \
        _mock_##name = nullptr;                                                                  \
    }                                                                                            \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7, A8 _8, A9 _9)                      \
    {                                                                                            \
        return _mock_##name                                                                      \
                   ? _mock_##name->name(_1, _2, _3, _4, _5, _6, _7, _8, _9)                      \
                   : default_lambda(_1, _2, _3, _4, _5, _6, _7, _8, _9);                         \
    }

// 10 引数 非 void 戻り値
#define MOCK_C_METHOD10_NONVOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda) \
    class Mock_##name                                                                                 \
    {                                                                                                 \
    public:                                                                                           \
        MOCK_METHOD10(name, ret(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10));                            \
        Mock_##name();                                                                                \
        ~Mock_##name();                                                                               \
    };                                                                                                \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD10_NONVOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, default_lambda) \
    Mock_##name *_mock_##name = nullptr;                                                               \
    Mock_##name::Mock_##name()                                                                         \
    {                                                                                                  \
        ON_CALL(*this, name(_, _, _, _, _, _, _, _, _, _))                                             \
            .WillByDefault(Invoke(default_lambda));                                                    \
        _mock_##name = this;                                                                           \
    }                                                                                                  \
    Mock_##name::~Mock_##name()                                                                        \
    {                                                                                                  \
        _mock_##name = nullptr;                                                                        \
    }                                                                                                  \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7, A8 _8, A9 _9, A10 _10)                   \
    {                                                                                                  \
        return _mock_##name                                                                            \
                   ? _mock_##name->name(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10)                       \
                   : default_lambda(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10);                          \
    }

// 0 引数 void 戻り値
#define MOCK_C_METHOD0_VOID_H(ret, name) \
    class Mock_##name                    \
    {                                    \
    public:                              \
        MOCK_METHOD0(name, ret());       \
        Mock_##name();                   \
        ~Mock_##name();                  \
    };                                   \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD0_VOID_CC(ret, name) \
    Mock_##name *_mock_##name = nullptr;  \
    Mock_##name::Mock_##name()            \
    {                                     \
        _mock_##name = this;              \
    }                                     \
    Mock_##name::~Mock_##name()           \
    {                                     \
        _mock_##name = nullptr;           \
    }                                     \
    ret name()                            \
    {                                     \
        if (_mock_##name)                 \
            _mock_##name->name();         \
    }

// 1 引数 void 戻り値
#define MOCK_C_METHOD1_VOID_H(ret, name, A1) \
    class Mock_##name                        \
    {                                        \
    public:                                  \
        MOCK_METHOD1(name, ret(A1));         \
        Mock_##name();                       \
        ~Mock_##name();                      \
    };                                       \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD1_VOID_CC(ret, name, A1) \
    Mock_##name *_mock_##name = nullptr;      \
    Mock_##name::Mock_##name()                \
    {                                         \
        _mock_##name = this;                  \
    }                                         \
    Mock_##name::~Mock_##name()               \
    {                                         \
        _mock_##name = nullptr;               \
    }                                         \
    ret name(A1 _1)                           \
    {                                         \
        if (_mock_##name)                     \
            _mock_##name->name(_1);           \
    }

// 2 引数 void 戻り値
#define MOCK_C_METHOD2_VOID_H(ret, name, A1, A2) \
    class Mock_##name                            \
    {                                            \
    public:                                      \
        MOCK_METHOD2(name, ret(A1, A2));         \
        Mock_##name();                           \
        ~Mock_##name();                          \
    };                                           \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD2_VOID_CC(ret, name, A1, A2) \
    Mock_##name *_mock_##name = nullptr;          \
    Mock_##name::Mock_##name()                    \
    {                                             \
        _mock_##name = this;                      \
    }                                             \
    Mock_##name::~Mock_##name()                   \
    {                                             \
        _mock_##name = nullptr;                   \
    }                                             \
    ret name(A1 _1, A2 _2)                        \
    {                                             \
        if (_mock_##name)                         \
            _mock_##name->name(_1, _2);           \
    }

// 3 引数 void 戻り値
#define MOCK_C_METHOD3_VOID_H(ret, name, A1, A2, A3) \
    class Mock_##name                                \
    {                                                \
    public:                                          \
        MOCK_METHOD3(name, ret(A1, A2, A3));         \
        Mock_##name();                               \
        ~Mock_##name();                              \
    };                                               \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD3_VOID_CC(ret, name, A1, A2, A3) \
    Mock_##name *_mock_##name = nullptr;              \
    Mock_##name::Mock_##name()                        \
    {                                                 \
        _mock_##name = this;                          \
    }                                                 \
    Mock_##name::~Mock_##name()                       \
    {                                                 \
        _mock_##name = nullptr;                       \
    }                                                 \
    ret name(A1 _1, A2 _2, A3 _3)                     \
    {                                                 \
        if (_mock_##name)                             \
            _mock_##name->name(_1, _2, _3);           \
    }

// 4 引数 void 戻り値
#define MOCK_C_METHOD4_VOID_H(ret, name, A1, A2, A3, A4) \
    class Mock_##name                                    \
    {                                                    \
    public:                                              \
        MOCK_METHOD4(name, ret(A1, A2, A3, A4));         \
        Mock_##name();                                   \
        ~Mock_##name();                                  \
    };                                                   \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD4_VOID_CC(ret, name, A1, A2, A3, A4) \
    Mock_##name *_mock_##name = nullptr;                  \
    Mock_##name::Mock_##name()                            \
    {                                                     \
        _mock_##name = this;                              \
    }                                                     \
    Mock_##name::~Mock_##name()                           \
    {                                                     \
        _mock_##name = nullptr;                           \
    }                                                     \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4)                  \
    {                                                     \
        if (_mock_##name)                                 \
            _mock_##name->name(_1, _2, _3, _4);           \
    }

// 5 引数 void 戻り値
#define MOCK_C_METHOD5_VOID_H(ret, name, A1, A2, A3, A4, A5) \
    class Mock_##name                                        \
    {                                                        \
    public:                                                  \
        MOCK_METHOD5(name, ret(A1, A2, A3, A4, A5));         \
        Mock_##name();                                       \
        ~Mock_##name();                                      \
    };                                                       \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD5_VOID_CC(ret, name, A1, A2, A3, A4, A5) \
    Mock_##name *_mock_##name = nullptr;                      \
    Mock_##name::Mock_##name()                                \
    {                                                         \
        _mock_##name = this;                                  \
    }                                                         \
    Mock_##name::~Mock_##name()                               \
    {                                                         \
        _mock_##name = nullptr;                               \
    }                                                         \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5)               \
    {                                                         \
        if (_mock_##name)                                     \
            _mock_##name->name(_1, _2, _3, _4, _5);           \
    }

// 6 引数 void 戻り値
#define MOCK_C_METHOD6_VOID_H(ret, name, A1, A2, A3, A4, A5, A6) \
    class Mock_##name                                            \
    {                                                            \
    public:                                                      \
        MOCK_METHOD6(name, ret(A1, A2, A3, A4, A5, A6));         \
        Mock_##name();                                           \
        ~Mock_##name();                                          \
    };                                                           \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD6_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6) \
    Mock_##name *_mock_##name = nullptr;                          \
    Mock_##name::Mock_##name()                                    \
    {                                                             \
        _mock_##name = this;                                      \
    }                                                             \
    Mock_##name::~Mock_##name()                                   \
    {                                                             \
        _mock_##name = nullptr;                                   \
    }                                                             \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6)            \
    {                                                             \
        if (_mock_##name)                                         \
            _mock_##name->name(_1, _2, _3, _4, _5, _6);           \
    }

// 7 引数 void 戻り値
#define MOCK_C_METHOD7_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7) \
    class Mock_##name                                                \
    {                                                                \
    public:                                                          \
        MOCK_METHOD7(name, ret(A1, A2, A3, A4, A5, A6, A7));         \
        Mock_##name();                                               \
        ~Mock_##name();                                              \
    };                                                               \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD7_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7) \
    Mock_##name *_mock_##name = nullptr;                              \
    Mock_##name::Mock_##name()                                        \
    {                                                                 \
        _mock_##name = this;                                          \
    }                                                                 \
    Mock_##name::~Mock_##name()                                       \
    {                                                                 \
        _mock_##name = nullptr;                                       \
    }                                                                 \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7)         \
    {                                                                 \
        if (_mock_##name)                                             \
            _mock_##name->name(_1, _2, _3, _4, _5, _6, _7);           \
    }

// 8 引数 void 戻り値
#define MOCK_C_METHOD8_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8) \
    class Mock_##name                                                    \
    {                                                                    \
    public:                                                              \
        MOCK_METHOD8(name, ret(A1, A2, A3, A4, A5, A6, A7, A8));         \
        Mock_##name();                                                   \
        ~Mock_##name();                                                  \
    };                                                                   \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD8_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8) \
    Mock_##name *_mock_##name = nullptr;                                  \
    Mock_##name::Mock_##name()                                            \
    {                                                                     \
        _mock_##name = this;                                              \
    }                                                                     \
    Mock_##name::~Mock_##name()                                           \
    {                                                                     \
        _mock_##name = nullptr;                                           \
    }                                                                     \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7, A8 _8)      \
    {                                                                     \
        if (_mock_##name)                                                 \
            _mock_##name->name(_1, _2, _3, _4, _5, _6, _7, _8);           \
    }

// 9 引数 void 戻り値
#define MOCK_C_METHOD9_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
    class Mock_##name                                                        \
    {                                                                        \
    public:                                                                  \
        MOCK_METHOD9(name, ret(A1, A2, A3, A4, A5, A6, A7, A8, A9));         \
        Mock_##name();                                                       \
        ~Mock_##name();                                                      \
    };                                                                       \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD9_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
    Mock_##name *_mock_##name = nullptr;                                      \
    Mock_##name::Mock_##name()                                                \
    {                                                                         \
        _mock_##name = this;                                                  \
    }                                                                         \
    Mock_##name::~Mock_##name()                                               \
    {                                                                         \
        _mock_##name = nullptr;                                               \
    }                                                                         \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7, A8 _8, A9 _9)   \
    {                                                                         \
        if (_mock_##name)                                                     \
            _mock_##name->name(_1, _2, _3, _4, _5, _6, _7, _8, _9);           \
    }

// 10 引数 void 戻り値
#define MOCK_C_METHOD10_VOID_H(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
    class Mock_##name                                                              \
    {                                                                              \
    public:                                                                        \
        MOCK_METHOD10(name, ret(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10));         \
        Mock_##name();                                                             \
        ~Mock_##name();                                                            \
    };                                                                             \
    extern Mock_##name *_mock_##name;

#define MOCK_C_METHOD10_VOID_CC(ret, name, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)  \
    Mock_##name *_mock_##name = nullptr;                                             \
    Mock_##name::Mock_##name()                                                       \
    {                                                                                \
        _mock_##name = this;                                                         \
    }                                                                                \
    Mock_##name::~Mock_##name()                                                      \
    {                                                                                \
        _mock_##name = nullptr;                                                      \
    }                                                                                \
    ret name(A1 _1, A2 _2, A3 _3, A4 _4, A5 _5, A6 _6, A7 _7, A8 _8, A9 _9, A10 _10) \
    {                                                                                \
        if (_mock_##name)                                                            \
            _mock_##name->name(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10);             \
    }
