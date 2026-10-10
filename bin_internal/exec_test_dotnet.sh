#!/bin/bash

# このスクリプトの絶対パス
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

# ワークスペースのディレクトリ
WORKSPACE_DIR=${WORKSPACE_DIR:-$(cd "$SCRIPT_DIR/../../.." && pwd)}
FILES_LANG=$(bash "$WORKSPACE_DIR/framework/makefw/bin_internal/get_files_lang.sh" "$WORKSPACE_DIR")

# プロジェクト名 (カレント ディレクトリ名から取得)
PROJECT_NAME=$(basename "$(pwd)")

# 出力ディレクトリ
OUTPUT_DIR=${OUTPUT_DIR:-bin}
RESULTS_DIR=results

# make のビルドと同じ SDK を使用する。実行ファイルのパスは単一の引数に保つ。
DOTNET_CMD=${DOTNET:-dotnet}

# 最終結果用変数
EXIT_CODE=0
SUCCESS_COUNT=0
WARNING_COUNT=0
FAILURE_COUNT=0

# テスト結果サマリー (個別テスト用)
test_summary=""
REPORT_STATE=$(mktemp -d)
SUMMARY_JOURNAL="$REPORT_STATE/summary"
function finish_markdown() {
    local status=$?
    if [ -f "$SUMMARY_JOURNAL" ]; then
        python3 "$SCRIPT_DIR/results_markdown.py" summary --input "$SUMMARY_JOURNAL" \
            --directory "$PROJECT_NAME" --counts-dir "$REPORT_STATE/counts" \
            --output "$RESULTS_DIR/all_tests/summary.md" || status=1
    fi
    rm -rf "$REPORT_STATE"
    trap - EXIT
    exit "$status"
}
trap finish_markdown EXIT

# tput を安全に実行するヘルパー関数
function safe_tput() {
    if [[ -n "$TERM" && "$TERM" != "dumb" ]]; then
        tput "$@" 2>/dev/null || true
    fi
}

# テスト一覧を取得
function list_tests() {
    "$DOTNET_CMD" test --list-tests --no-build -c "$CONFIG" -o "$OUTPUT_DIR" 2> "$REPORT_STATE/list_error" | \
        grep -E '^\s+' | \
        sed -e 's/^[ \t]*//'
    return ${PIPESTATUS[0]}
}

# テストを一括実行して結果をパース
function run_all_tests_batch() {
    echo -e "Test start on $(export LANG=C && date)." | tee "$SUMMARY_JOURNAL"
    echo -e "----" | tee -a "$SUMMARY_JOURNAL"

    # テスト一覧を取得 (パラメーター付きテストは重複を除去)
    local tests
    tests=$(list_tests)
    local list_exit_code=$?
    tests=$(printf '%s\n' "$tests" | sed 's/(.*//' | sort -u)
    # テスト ホストとの通信に失敗しても、dotnet test --list-tests は終了コード 0 で
    # 一覧を空にすることがある。0 件を成功扱いにすると、テストを実行していないのに
    # 成功のスタンプが残るため、失敗として扱う。
    local list_error=""
    if [ "$list_exit_code" -ne 0 ]; then
        list_error="Error: dotnet test --list-tests failed with exit code $list_exit_code."
    elif [ -z "$tests" ]; then
        list_error="Error: dotnet test --list-tests found no tests."
        list_exit_code=1
    fi
    if [ -n "$list_error" ]; then
        cat "$REPORT_STATE/list_error" >&2
        echo "$list_error" >> "$SUMMARY_JOURNAL"
        echo -e "\e[31m$list_error\e[0m" >&2
        return "$list_exit_code"
    fi

    local test_count=$(echo "$tests" | wc -l)
    echo "Found $test_count test(s)."
    #echo "Test results:" | tee -a "$SUMMARY_JOURNAL"
    safe_tput cr

    # dotnet test を 1 回だけ一括実行
    local trx_dir=$(mktemp -d)
    local batch_output=$(mktemp)
    local batch_exit_code=0

    echo "Running all tests in batch mode..." > "$batch_output"
    "$DOTNET_CMD" test \
        --no-build -c "$CONFIG" -o "$OUTPUT_DIR" \
        --verbosity normal \
        --logger "trx;LogFileName=results.trx" \
        --results-directory "$trx_dir" >> "$batch_output" 2>&1
    batch_exit_code=$?

    # バッチ実行時の dotnet test 出力を表示 (失敗時のみ)
    if [ $batch_exit_code -ne 0 ]; then
        cat "$batch_output"
        printf 'Error: dotnet test failed with exit code %s.\n' "$batch_exit_code" >> "$SUMMARY_JOURNAL"
        python3 "$SCRIPT_DIR/results_markdown.py" decode --input "$batch_output" --encoding "${FILES_LANG#*.}" >> "$SUMMARY_JOURNAL"
        echo ""
        echo -e "\e[31mError: dotnet test failed with exit code $batch_exit_code.\e[0m" >&2
        rm -f "$batch_output"
        rm -rf "$trx_dir"
        echo ""
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        echo ""
        return $batch_exit_code
    fi

    # TRX ファイルを検索
    local trx_file=$(find "$trx_dir" -name "results.trx" -type f | head -1)
    if [ -z "$trx_file" ]; then
        echo -e "\e[31mError: TRX file not found in $trx_dir\e[0m" | tee -a "$SUMMARY_JOURNAL" >&2
        rm -f "$batch_output"
        rm -rf "$trx_dir"
        echo ""
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        echo ""
        return 1
    fi

    # TRX を解析してテストごとの結果を取得
    local trx_results=$(mktemp)
    if ! python3 "$SCRIPT_DIR/parse_trx_results.py" "$trx_file" --with-counts > "$trx_results" 2> "$REPORT_STATE/trx_error"; then
        cat "$REPORT_STATE/trx_error" | tee -a "$SUMMARY_JOURNAL" >&2
        rm -f "$batch_output" "$trx_results"
        rm -rf "$trx_dir"
        return 1
    fi

    # 各テストについてループ処理
    for test in $tests; do
        # パラメーター付きテストの場合、パラメーター部分を除去
        local fqn_base=$(echo "$test" | sed 's/(.*//')

        # クラス名とメソッド名を分離
        local namespace_and_class="${fqn_base%.*}"
        local method_name="${fqn_base##*.}"
        local class_name="${namespace_and_class##*.}"

        # results ディレクトリを作成
        local test_id="$class_name.$method_name"
        mkdir -p "$RESULTS_DIR/$test_id"

        local temp_file=$(mktemp)

        local evidence_file="$REPORT_STATE/evidence"
        local evidence_error="$REPORT_STATE/evidence_error"
        : > "$evidence_file"
        : > "$evidence_error"
        printf '@test\t%s\n' "$test_id" >> "$SUMMARY_JOURNAL"

        # テスト ファイルを探す
        local test_file=$(find . -name "${class_name}.cs" -type f | head -1)

        local test_result record_count record_status
        IFS=$'\t' read -r _ test_result record_count record_status < <(
            awk -F '\t' -v id="$test_id" '$1 == id { print; exit }' "$trx_results"
        )
        local evidence_failed=0
        local -a summary_options=(--test-id "$test_id" --encoding "${FILES_LANG#*.}" --counts-output "$REPORT_STATE/counts/$test_id.json")
        if [[ "$record_count" =~ ^[1-9][0-9]*$ ]]; then
            summary_options+=(--param-count "$record_count")
        fi
        if [ "$record_status" = "Partial" ]; then
            summary_options+=(--partial)
        fi
        if [ -n "$test_file" ]; then
            # 抽出・解析のどちらの失敗も実行結果へ反映する。
            if ! (set -o pipefail
                python3 "$SCRIPT_DIR/test_subprocedures.py" --language dotnet \
                    --source "$test_file" "${summary_options[@]}"
            ) > "$evidence_file" 2> "$evidence_error"; then
                evidence_failed=1
            fi
        else
            # ソース探索と抽出の失敗も、エビデンス生成の失敗として記録する。
            printf '[  FAILED  ] %s: Test source was not found.\n' "$test_id" > "$evidence_error"
            evidence_failed=1
        fi
        if [ "$evidence_failed" -ne 0 ]; then
            test_result="Failed"
        elif [ -z "$test_result" ]; then
            test_result="Failed"
            printf '%s\n' '[  FAILED  ] Test result was not found in TRX.' >> "$evidence_error"
        fi

        # バッチ出力から該当テスト分を抽出
        python3 "$SCRIPT_DIR/extract_dotnet_output.py" "$batch_output" "$test_id" "$test_result" "${FILES_LANG#*.}" > "$temp_file"

        # 結果を判定
        if [ "$test_result" = "Passed" ]; then
            if grep -qE "\[ *WARNING *\]" "$temp_file"; then
                test_summary+="$(echo -e "$test_id\t\e[33mWARNING\e[0m")"$'\n'
                echo -e "$test_id\tWARNING" >> "$SUMMARY_JOURNAL"
                WARNING_COUNT=$((WARNING_COUNT + 1))
            else
                test_summary+="$(echo -e "$test_id\t\e[32mPASSED\e[0m")"$'\n'
                echo -e "$test_id\tPASSED" >> "$SUMMARY_JOURNAL"
                SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
            fi
        else
            test_summary+="$(echo -e "$test_id\t\e[31mFAILED\e[0m")"$'\n'
            echo -e "$test_id\tFAILED" >> "$SUMMARY_JOURNAL"
            FAILURE_COUNT=$((FAILURE_COUNT + 1))
            EXIT_CODE=1
        fi

        local final_status=FAILED
        if [ "$test_result" = "Passed" ]; then
            final_status=PASSED
            if grep -qE "\[ *WARNING *\]" "$temp_file"; then final_status=WARNING; fi
        fi
        local -a markdown_options=(--test-id "$test_id" --status "$final_status"
            --evidence "$evidence_file" --input "$temp_file" --encoding utf-8)
        if [ -s "$evidence_error" ]; then markdown_options+=(--error "$evidence_error"); fi
        # 抽出した実行結果は UTF-8。色付け後に端末の文字コードで表示する。
        local console_file="$REPORT_STATE/console"
        python3 "$SCRIPT_DIR/results_markdown.py" individual "${markdown_options[@]}" \
            --output "$RESULTS_DIR/$test_id/results.md" --console-output "$console_file" \
            --console-encoding "${FILES_LANG#*.}" --dotnet-color
        cat "$console_file"
        echo ""
        grep -E '\[ *WARNING *\]|\[ *FAILED *\]|^Error:' "$temp_file" >> "$SUMMARY_JOURNAL" || true
        if [ -s "$evidence_error" ]; then cat "$evidence_error" >> "$SUMMARY_JOURNAL"; fi
        rm -f "$temp_file"
    done

    # 一時ファイルのクリーンアップ
    rm -f "$batch_output" "$trx_results"
    rm -rf "$trx_dir"

    printf '@test\t\n' >> "$SUMMARY_JOURNAL"

    # テスト結果サマリーを表示
    echo "----"
    printf "%s" "$test_summary"
    # 集計結果を出力
    echo "----" | tee -a "$SUMMARY_JOURNAL"
    printf "Total tests\t%d\n" $((SUCCESS_COUNT + WARNING_COUNT + FAILURE_COUNT)) | tee -a "$SUMMARY_JOURNAL"
    printf "Passed\t\t%d\n" $SUCCESS_COUNT | tee -a "$SUMMARY_JOURNAL"
    printf "Warning(s)\t%d\n" $WARNING_COUNT | tee -a "$SUMMARY_JOURNAL"
    printf "Failed\t\t%d\n" $FAILURE_COUNT | tee -a "$SUMMARY_JOURNAL"
    echo ""

    # Banner 表示
    if [ $EXIT_CODE -eq 0 ]; then
        bash "$SCRIPT_DIR/banner.sh" PASSED "\e[32m"
        echo ""
    else
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        echo ""
    fi

    return $EXIT_CODE
}

# メイン処理
function main() {
    # 既存の results ディレクトリを削除して新規作成
    rm -rf "$RESULTS_DIR"
    mkdir -p "$RESULTS_DIR/all_tests"

    # テストを一括実行
    run_all_tests_batch
    return $?
}

# 実行
main
exit $?
