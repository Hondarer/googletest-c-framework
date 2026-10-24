#!/bin/bash

# テストの省略判定の対象とするスクリプトの絶対パスを、1 行に 1 つ名前順に出力する。
# 範囲はこのスクリプトだけで定義し、次の 2 つの判定が共有する。
#   - exec_test_c_cpp.sh の compute_test_signature (テスト ディレクトリ単位)
#   - framework/makefw/bin_internal/resolve_app_deps.sh --signature (app 直下)
#
# 対象は testfw の bin_internal/ 直下の *.py と *.sh すべて。
# 結果に影響しないスクリプトも含めるのは、一覧の更新漏れで古い結果を再利用しないため。
# テストの実行スクリプトが使う makefw のスクリプトは、makefw の list_signature_inputs.sh が定義する。

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P) || exit

{
    for path in "$script_dir"/*.py "$script_dir"/*.sh; do
        [ -f "$path" ] && printf '%s\n' "$path"
    done
} | LC_ALL=C sort
