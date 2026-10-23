#!/bin/bash

# テストの省略判定の対象とするスクリプトの絶対パスを、1 行に 1 つ名前順に出力する。
# 範囲はこのスクリプトだけで定義し、次の 2 つの判定が共有する。
#   - exec_test_c_cpp.sh の compute_test_signature (テスト ディレクトリ単位)
#   - framework/makefw/bin_internal/resolve_app_deps.sh --signature (app 直下)
#
# 対象は testfw の bin_internal/ 直下の *.py と *.sh すべてと、テストの実行スクリプトが
# 結果の文字コードを決めるために呼ぶ makefw の get_files_lang.sh。
# 結果に影響しないスクリプトも含めるのは、一覧の更新漏れで古い結果を再利用しないため。
# makefw は testfw と同じ親ディレクトリ (framework/) にあるものとする。

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P) || exit
framework_dir=$(cd "$script_dir/../.." && pwd -P) || exit

{
    for path in "$script_dir"/*.py "$script_dir"/*.sh \
        "$framework_dir/makefw/bin_internal/get_files_lang.sh"; do
        [ -f "$path" ] && printf '%s\n' "$path"
    done
} | LC_ALL=C sort
