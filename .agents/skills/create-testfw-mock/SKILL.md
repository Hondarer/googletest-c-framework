---
name: create-testfw-mock
description: framework/testfw の標準ライブラリ mock を追加または変更するときに使用します。include_override、呼び出し元情報、Mock クラス、real と fake delegate、ON_CALL、影響テストを扱います。
---

# testfw mock の作成

1. `framework/testfw/docs/how-to-mock.md` を読んでください。mock 関数本体の中継は `mock_ret`、テスト本体の受けは `actual_ret` です。
2. `framework/testfw/AGENTS.md` と同じカテゴリの既存 mock を確認してください。
3. override ヘッダー、mock 宣言、`MOCK_METHOD`、`ON_CALL`、関数実装を更新してください。
4. 置換マクロには、既存規則に従って file、line、func を追加してください。
5. 未注入時は real delegate、必要な切り替えでは fake delegate を使用してください。
6. 新しい override ヘッダーを追加した場合は、影響対象のオブジェクトを再生成してください。
7. testfw の局所テストと、影響する app テストを確認してください。

testfw mock の関数へ `WEAK_ATR` を追加しないでください。
