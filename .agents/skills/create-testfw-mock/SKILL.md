---
name: create-testfw-mock
description: framework/testfw の標準ライブラリ mock を追加・変更し、呼び出し元情報と real・fake delegate の規則を維持します。
---

# testfw mock の作成

`framework/testfw/docs/how-to-mock.md` の対象カテゴリの節を、既存 mock と照合してください。  
override ヘッダー、mock 宣言、`MOCK_METHOD`、`ON_CALL`、関数実装のシグネチャを一致させてください。

置換マクロには既存規則に従って file、line、func を追加してください。  
未注入時は real delegate、必要な切り替えでは fake delegate を使用し、testfw mock の関数へ `WEAK_ATR` を追加しないでください。  
新しい override ヘッダーを追加した場合は、影響対象のオブジェクトを再生成してください。

変更したカテゴリの局所テストを実行してください。  
利用側でしか確認できない影響がある場合は、該当する app テストも選んでください。  
全体テストの実行条件はワークスペースの指示に従ってください。
