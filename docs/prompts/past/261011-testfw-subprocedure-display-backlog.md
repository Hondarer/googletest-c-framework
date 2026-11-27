# 残課題一覧 (バックログ)

ファイル名: `prompt/testfw-subprocedure-display-backlog.md`

## 案件の識別

案件名: testfw のテスト結果におけるサブ手順表示と空行  
作成日: 2026-10-11  
参照設計書: `prompt/testfw-subprocedure-display-design.md` (版 2)  
参照検討文書: `prompt/testfw-subprocedure-display-discussion.md` (版 13)

この文書は、設計および実装・検証作業がすべて完了した後に、作業中に別問題として判明したが今回は対策しなかった残課題を記録したものです。作業中に解消した問題や、仕様・制約として容認した問題点は含みません。

## 残課題一覧

Table: 残課題の一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | テスト前のサブ手順参照がテストの説明を消す | 作業中摘出 | 中 | 完了 |

## BL-001 テスト前のサブ手順参照がテストの説明を消す

> [!NOTE]
> 2026-10-11 に `prompt/testfw-results-refinement-design.md` (版 2) で解消しました。

### 概要と問題点

- **現状の事象**: テスト定義 (`TEST_F` など) の直前にある説明コメントと、テスト定義との間に `[サブ手順参照]` の行があると、説明コメントがテスト項目節に出力されません。
    - 例: `app/calc/test/src/libcalcTest/exportTest/exportTest.cc` の `exportTest.symbol_names_match` は、説明コメント「libcalc のエクスポート シンボル名が API 表と一致することの確認」の次の行に `// [サブ手順参照 名前=exportTest.SetUp]` があります。生成された `results.md` のテスト項目節には、この説明がありません。
    - 原因: `framework/testfw/bin_internal/test_summary.py` の `render_summary` は、テスト定義より前の行でタグを含むコメントを見つけると、それまでに集めた説明を破棄します。サブ手順参照を展開した行もタグを含むため、説明が破棄されます。
- **本来あるべき姿**: `about-test-phase.md` の「テスト項目」節の規則 (テスト定義の直前に空行を挟まず書いた `//` コメントが概要になる) と、`test-subprocedures.md` の「前後の参照はテストに空行を挟まず置く」の規則を同時に満たすとき、説明コメントが概要として出力されること。

### 経緯と見送り理由

- **摘出契機**: 実装後の受入確認で、指摘箇所の `results.md` を確認した際に発見しました。
- **今回の見送り理由**: 今回の範囲 (サブ手順の入れ子表示、回数、並び順、空行) の外にあり、変更前から存在する事象です。

### 影響範囲とリスク

- **影響対象**: テスト定義の直前にサブ手順参照 (fixture の `SetUp` など) を置いたすべてのテストの `results.md`。app 全体で、サブ手順参照の直前の行が通常のコメントである箇所は約 970 か所あります (概算)。
- **放置した場合のリスク**: テストの目的を説明する文がエビデンスに残らず、レビューでテストの意図を読み取りにくくなります。

### 推奨される対応策

- **恒久対応案**: `render_summary` で、テスト定義より前にあるサブ手順展開の範囲 (`[サブ手順展開 …]` から `[サブ手順展開終了]` まで) では説明の破棄を行わないようにします。説明コメントとサブ手順参照の順序 (説明が先、参照が後) を正本文書に明記し、`bin_test/test_subprocedure_reports.py` に説明が残ることのテストを追加します。
- **参考情報**: `framework/testfw/bin_internal/test_summary.py` の `render_summary` (`desc` の収集処理)、`framework/testfw/docs/about-test-phase.md` の「テスト項目」節、`framework/testfw/docs/test-subprocedures.md` の「前後の参照はテストに空行を挟まず置く」節
