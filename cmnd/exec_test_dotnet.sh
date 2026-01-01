#!/bin/bash

# このスクリプトのパス (dirname "$0" 相当)
SCRIPT_DIR=${0%/*}

# ワークスペースのディレクトリ
WORKSPACE_FOLDER=$SCRIPT_DIR/../../

# プロジェクト名 (カレントディレクトリ名から取得)
PROJECT_NAME=$(basename "$(pwd)")

# 出力ディレクトリ
OUTPUT_DIR=${OUTPUT_DIR:-bin}
RESULTS_DIR=results

# 最終結果用変数
EXIT_CODE=0
SUCCESS_COUNT=0
WARNING_COUNT=0
FAILURE_COUNT=0

# テスト結果サマリ (個別テスト用)
test_summary=""

# tput を安全に実行するヘルパー関数
function safe_tput() {
    if [[ -n "$TERM" && "$TERM" != "dumb" ]]; then
        tput "$@" 2>/dev/null || true
    fi
}

# テスト一覧を取得
function list_tests() {
    dotnet test --list-tests --no-build -c "$CONFIG" -o "$OUTPUT_DIR" 2>/dev/null | \
        grep -E '^\s+' | \
        sed -e 's/^[ \t]*//'
}

# 個別テストを実行
function run_test() {
    local fully_qualified_name=$1

    # パラメータ付きテストの場合、パラメータ部分を除去
    # 例: CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: 10, b: 20, expected: 30)
    #  -> CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult
    local fqn_base=$(echo "$fully_qualified_name" | sed 's/(.*//')

    # クラス名とメソッド名を分離
    # 例: CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult
    local namespace_and_class="${fqn_base%.*}"  # CalcLib.Tests.CalcLibraryTests
    local method_name="${fqn_base##*.}"          # Add_ShouldReturnCorrectResult
    local class_name="${namespace_and_class##*.}" # CalcLibraryTests

    # results ディレクトリを作成
    local test_id="$class_name.$method_name"
    mkdir -p "$RESULTS_DIR/$test_id"

    local temp_file=$(mktemp)
    local temp_exit_code=$(mktemp)

    echo -e "Running test: $test_id" > "$temp_file"
    echo -e "----" >> "$temp_file"

    # テストファイルを探す
    local test_file=$(find . -name "${class_name}.cs" -type f | head -1)

    if [ -n "$test_file" ]; then
        # テストコードを抽出してサマリを生成
        python3 "$SCRIPT_DIR/get_test_code_dotnet.py" "$test_file" "$class_name" "$method_name" 2>/dev/null | \
            python3 "$SCRIPT_DIR/insert_summary_dotnet.py" >> "$temp_file"
        echo -e "----" >> "$temp_file"
    fi

    # テストを実行
    echo "dotnet test --filter \"FullyQualifiedName~$class_name.$method_name\"" >> "$temp_file"
    echo "" >> "$temp_file"

    if dotnet test --filter "FullyQualifiedName~$class_name.$method_name" \
        --no-build -c "$CONFIG" -o "$OUTPUT_DIR" --verbosity normal 2>&1 | \
        grep -v '^\[xUnit\.net' | \
        grep -v 'にビルドを開始しました' | \
        grep -v 'Build started' | \
        grep -v 'ビルドに成功しました' | \
        grep -v 'Build succeeded' | \
        grep -v '^\s*[0-9]\+\s*個の警告' | \
        grep -v '^\s*[0-9]\+\s*Warning(s)' | \
        grep -v '^\s*[0-9]\+\s*エラー' | \
        grep -v '^\s*[0-9]\+\s*Error(s)' | \
        grep -v '経過時間' | \
        grep -v 'Time Elapsed' | \
        cat -s >> "$temp_file"; then
        echo 0 > "$temp_exit_code"
    else
        echo $? > "$temp_exit_code"
    fi

    # 結果を判定
    local result=$(<"$temp_exit_code")
    rm -f "$temp_exit_code"

    if [ $result -eq 0 ]; then
        if grep -q "WARNING" "$temp_file"; then
            test_summary+="$(echo -e "$test_id\t\e[33mWARNING\e[0m")"$'\n'
            echo -e "$test_id\tWARNING" >> "$RESULTS_DIR/all_tests/summary.log"
            WARNING_COUNT=$((WARNING_COUNT + 1))
        else
            test_summary+="$(echo -e "$test_id\t\e[32mPASSED\e[0m")"$'\n'
            echo -e "$test_id\tPASSED" >> "$RESULTS_DIR/all_tests/summary.log"
            SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
        fi
    else
        test_summary+="$(echo -e "$test_id\t\e[31mFAILED\e[0m")"$'\n'
        echo -e "$test_id\tFAILED" >> "$RESULTS_DIR/all_tests/summary.log"
        FAILURE_COUNT=$((FAILURE_COUNT + 1))
        EXIT_CODE=1
    fi

    # コンソールに表示 (色コードあり、末尾の空行を削除、メッセージに着色)
    cat "$temp_file" | \
        sed -e 's/^テストの実行に成功しました。$/\x1b[32m&\x1b[0m/' | \
        sed -e 's/^Test Run Successful\.$/\x1b[32m&\x1b[0m/' | \
        sed -e 's/^テストの実行に失敗しました。$/\x1b[31m&\x1b[0m/' | \
        sed -e 's/^Test Run Failed\.$/\x1b[31m&\x1b[0m/' | \
        sed -e :a -e '/^\s*$/{ $d; N; ba; }'
    echo ""

    # results.log に保存 (色コードを除去し、末尾の空行を削除)
    cat "$temp_file" | sed -r 's/\x1b\[[0-9;]*m//g' | sed -e :a -e '/^\s*$/{ $d; N; ba; }' > "$RESULTS_DIR/$test_id/results.log"
    rm -f "$temp_file"
}

# テストを実行
function run_individual_tests() {
    echo -e "Test start on $(export LANG=C && date)." | tee "$RESULTS_DIR/all_tests/summary.log"
    echo -e "----" | tee -a "$RESULTS_DIR/all_tests/summary.log"

    # テスト一覧を取得 (パラメータ付きテストは重複を除去)
    local tests=$(list_tests | sed 's/(.*//' | sort -u)

    if [ -z "$tests" ]; then
        echo "No tests found."
        return 1
    fi

    local test_count=$(echo "$tests" | wc -l)
    echo "Found $test_count tests." | tee -a "$RESULTS_DIR/all_tests/summary.log"
    echo "" | tee -a "$RESULTS_DIR/all_tests/summary.log"
    safe_tput cr

    # 各テストを実行
    for test in $tests; do
        run_test "$test"
    done

    # テスト結果サマリを表示
    printf "%s" "$test_summary"
    # 集計結果を出力
    echo "----" | tee -a "$RESULTS_DIR/all_tests/summary.log"
    printf "Total tests\t%d\n" $((SUCCESS_COUNT + WARNING_COUNT + FAILURE_COUNT)) | tee -a "$RESULTS_DIR/all_tests/summary.log"
    printf "Passed\t\t%d\n" $SUCCESS_COUNT | tee -a "$RESULTS_DIR/all_tests/summary.log"
    printf "Warning(s)\t%d\n" $WARNING_COUNT | tee -a "$RESULTS_DIR/all_tests/summary.log"
    printf "Failed\t\t%d\n" $FAILURE_COUNT | tee -a "$RESULTS_DIR/all_tests/summary.log"
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

    # テストを実行
    run_individual_tests
    return $?
}

# 実行
main
exit $?
