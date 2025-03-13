#!/bin/bash

# このスクリプトのパス
SCRIPT_DIR=$(dirname "$0")

# ワークスペースのディレクトリ
WORKSPACE_FOLDER=$SCRIPT_DIR/../../

# ソースファイルのエンコード指定から LANG を得る
FILES_LANG=$(sh $SCRIPT_DIR/get_files_lang.sh $WORKSPACE_FOLDER)

# テストバイナリのパス
TEST_BINARY=$(basename `pwd`)

# スタックサイズ制限緩和
# (1) ハードリミットのスタックサイズを取得
hard_limit=$(ulimit -H -s)
# (2) ハードリミットのスタックサイズをソフトリミットに設定
ulimit -s "$hard_limit"

# テスト一覧を取得
function list_tests() {
    ./$TEST_BINARY --gtest_list_tests | awk '
    /^[^ ]/ {suite=$1} 
    /^  / {print suite substr($0, 3)}'
}

# テストを実行 (個別カバレッジあり)
function run_test() {
    local test_comment=""
    local test_comment_delim=""
    if [[ "$1" == *#* ]]; then
        test_comment_delim=" "
        test_comment="#${1#*#}"
    fi
    local test_name=$(echo "$1" | cut -d' ' -f1)

    # 階層構造の管理上の都合で
    # パラメータテストの prefix をテストクラスの後に付けた ID を生成する
    # test_name: google test で内部的に扱うテスト名 (パラメータの prefix がテストクラスの前に付与されているもの)
    # test_id: 人間系に見せるテスト名 (パラメータの prefix がテストクラス名の後、パラメータ名の前に付与されているもの)
    local test_id
    if [[ $(awk -F'/' '{print NF-1}' <<< "$test_name") -eq 2 ]]; then
        test_id=$(echo "$test_name" | awk -F'/' '{print $2"/"$1"/"$3}')
    else
        test_id=$(echo "$test_name")
    fi

    make clean-cov > /dev/null
    mkdir -p results/$test_id
    local temp_file=$(mktemp)
    local temp_exit_code=$(mktemp)

    echo -e "\nRunning test: $test_id$test_comment_delim$test_comment on $TEST_BINARY"
    tput cr
    echo -e "Running test: $test_id$test_comment_delim$test_comment on $TEST_BINARY" > $temp_file

    # テストコードに着色する場合:
    # cat *.cc *.cpp 2>/dev/null | awk -v test_name=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk | source-highlight -s cpp -f esc;
    LANG=$FILES_LANG script -q -a -c \
       "echo \"----\"; \
        cat *.cc *.cpp 2>/dev/null | awk -v test_id=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk; \
        echo \"----\"; \
        echo ./$TEST_BINARY --gtest_filter=\"$test_name\"; \
        ./$TEST_BINARY --gtest_color=yes --gtest_filter=\"$test_name\" 2>&1 | grep -v \"Note: Google Test filter\"; \
        exit_code=\${PIPESTATUS[0]}; \
        if [ \$exit_code -ge 128 ]; then \
            signal=\$((exit_code - 128)); \
            echo -n -e \"\\n\\e[31m[  FAILED  ]\\e[0m Terminated by signal \$signal, \"; \
            case \$signal in \
                6)  echo \"SIGABRT: abort.\";; \
                11) echo \"SIGSEGV: segmentation fault.\";; \
                8)  echo \"SIGFPE: floating-point exception.\";; \
                4)  echo \"SIGILL: illegal instruction.\";; \
                *)  echo \"Abnormal termination by other signal.\";; \
            esac; \
        fi; \
        echo \$exit_code > $temp_exit_code" $temp_file

    local result=$(cat $temp_exit_code)
    rm -f $temp_exit_code
    cat $temp_file | sed -r 's/\x1b\[[0-9;]*m//g' > results/$test_id/results.log
    rm -f $temp_file
    make take-gcov 1> /dev/null 2>&1

    if ls gcov/*.gcov 1> /dev/null 2>&1; then
        for file in gcov/*.gcov; do
            cp -p "$file" "results/$test_id/$(basename "$file").txt"
        done
    fi

    return $result
}

# メイン処理
function main() {
    rm -rf results
    mkdir results
    mkdir -p results/all_tests

    # google test は、GTEST_FILTER が定義されている場合は空文字でもフィルタを行う
    # そのため、指定があるかどうかは環境変数の有無をチェックする必要がある
    if [[ "${GTEST_FILTER+x}" ]]; then
        echo -e "\e[33m"
            echo "Note: GTEST_FILTER = $GTEST_FILTER"
        echo -e "\e[0m"
    fi

    # テスト対象ソースの md5 を取得
    echo -e "Test start on $(export LANG=C && date)." | tee -a results/all_tests/summary.log
    tput cr
    echo -e "----" | tee -a results/all_tests/summary.log
    tput cr
    echo -e "MD5 checksums of files in TEST_SRCS:" | tee -a results/all_tests/summary.log
    tput cr
    for src in $TEST_SRCS; do
        md5sum "$src" | sed -e "s#$(realpath "$WORKSPACE_FOLDER")/##g" | tee -a results/all_tests/summary.log
        tput cr
    done
    echo "----" | tee -a results/all_tests/summary.log
    tput cr

    tests=$(list_tests)
    #tests=$(echo "$tests" | sort)
    test_count=$(echo "$tests" | wc -l)
    if [[ -z "$tests" ]]; then
        test_count=0
    fi
    echo "Found $test_count tests."
    tput cr

    IFS=$'\n'
        for test_name_w_comment in $tests; do
            run_test "$test_name_w_comment"
            # すべてのテストをやり切ったほうが使い勝手が良い
            # 失敗しない前提であれば、以下を活かしても良い
            #local result=$?
            #if [ "$result" -ne 0 ]; then
            #    return 1
            #fi
        done
    unset IFS

    # すべてのテストを通しで実行後、全体カバレッジを取得
    # プロセスの依存性排除のため、一括ではなく個別にテストを実施
    SUCCESS_COUNT=0
    WARNING_COUNT=0
    FAILURE_COUNT=0
    make clean-cov > /dev/null

    echo -e ""
    
    local first_loop=0

    if [[ "${GTEST_FILTER+x}" ]]; then
        echo -e "Note: GTEST_FILTER = $GTEST_FILTER\n" >> results/all_tests/results.log
        echo -e "Note: GTEST_FILTER = $GTEST_FILTER\n" >> results/all_tests/summary.log
    fi

    echo "Test results:" >> results/all_tests/summary.log

    IFS=$'\n'
        for test_name_w_comment in $tests; do
            local temp_file=$(mktemp)
            local temp_exit_code=$(mktemp)
            local test_comment=""
            local test_comment_delim=""
            if [[ "$test_name_w_comment" == *#* ]]; then
                test_comment_delim=" "
                test_comment="#${test_name_w_comment#*#}"
            fi
            local test_name=$(echo "$test_name_w_comment" | cut -d' ' -f1)

            # 階層構造の管理上の都合で
            # パラメータテストの prefix をテストクラスの後に付けた ID を生成する
            # test_name: google test で内部的に扱うテスト名 (パラメータの prefix がテストクラスの前に付与されているもの)
            # test_id: 人間系に見せるテスト名 (パラメータの prefix がテストクラス名の後、パラメータ名の前に付与されているもの)
            local test_id
            if [[ $(awk -F'/' '{print NF-1}' <<< "$test_name") -eq 2 ]]; then
                test_id=$(echo "$test_name" | awk -F'/' '{print $2"/"$1"/"$3}')
            else
                test_id=$(echo "$test_name")
            fi

            if [ $first_loop -eq 0 ]; then
                first_loop=1
            else
                echo "" > $temp_file
            fi

            echo -e "Running test: $test_id$test_comment_delim$test_comment on $TEST_BINARY" >> $temp_file

            LANG=$FILES_LANG script -q -a -c \
               "echo \"----\"; \
                cat *.cc *.cpp 2>/dev/null | awk -v test_id=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk; \
                echo \"----\"; \
                echo ./$TEST_BINARY --gtest_filter=\"$test_name\"; \
                ./$TEST_BINARY --gtest_filter=\"$test_name\" 2>&1 | grep -v \"Note: Google Test filter\"; \
                exit_code=\${PIPESTATUS[0]}; \
                if [ \$exit_code -ge 128 ]; then \
                    signal=\$((exit_code - 128)); \
                    echo -n -e \"\\n\\e[31m[  FAILED  ]\\e[0m Terminated by signal \$signal, \"; \
                    case \$signal in \
                        6)  echo \"SIGABRT: abort.\";; \
                        11) echo \"SIGSEGV: segmentation fault.\";; \
                        8)  echo \"SIGFPE: floating-point exception.\";; \
                        4)  echo \"SIGILL: illegal instruction.\";; \
                        *)  echo \"Abnormal termination by other signal.\";; \
                    esac; \
                fi; \
                echo \$exit_code > $temp_exit_code" $temp_file > /dev/null

            local result=$(cat $temp_exit_code)
            rm -f $temp_exit_code
            if [ $result -eq 0 ]; then
                if grep -q "WARNING" $temp_file; then
                    echo -e "$test_id\t\e[33mWARNING\e[0m\t$test_comment"
                    echo -e "$test_id\tWARNING\t$test_comment" >> results/all_tests/summary.log
                    WARNING_COUNT=$((WARNING_COUNT + 1))
                else
                    echo -e "$test_id\t\e[32mPASSED\e[0m\t$test_comment"
                    echo -e "$test_id\tPASSED\t$test_comment" >> results/all_tests/summary.log
                    SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
                fi
            else
                echo -e "$test_id\t\e[31mFAILED\e[0m\t$test_comment"
                echo -e "$test_id\tFAILED\t$test_comment" >> results/all_tests/summary.log
                FAILURE_COUNT=$((FAILURE_COUNT + 1))
            fi
            cat $temp_file | sed -r 's/\x1b\[[0-9;]*m//g' >> results/all_tests/results.log
            rm -f $temp_file
        done
    unset IFS

    filtered=""
    if [[ "${GTEST_FILTER+x}" ]]; then
        filtered=" (filtered, $GTEST_FILTER)"
    fi

    echo -e "----\nTotal tests\t$test_count\e[33m$filtered\e[0m\nPassed\t\t$SUCCESS_COUNT\nWarning(s)\t$WARNING_COUNT\nFailed\t\t$FAILURE_COUNT"
    echo -e "----\nTotal tests\t$test_count$filtered\nPassed\t\t$SUCCESS_COUNT\nWarning(s)\t$WARNING_COUNT\nFailed\t\t$FAILURE_COUNT" >> results/all_tests/summary.log
    make take-gcov take-lcov 1> /dev/null 2>&1

    if ls gcov/*.gcov 1> /dev/null 2>&1; then
        for file in gcov/*.gcov; do
            cp -p "$file" "results/all_tests/$(basename "$file").txt"
        done
    fi

    if ls lcov/* 1> /dev/null 2>&1; then
        cp -rp lcov results/all_tests/.

        # FILES_LANG が utf-8 でない場合の処理
        if [[ ! "${FILES_LANG}" =~ [Uu][Tt][Ff][-+_]*8 ]]; then
            find results/all_tests/lcov -name "*.gcov.html" | while read -r file; do
                sed -i "s/charset=UTF-8/charset=${FILES_LANG#*.}/" "$file"
            done
        fi
    fi

    echo "" | tee -a results/all_tests/summary.log
    make --no-print-directory take-gcovr 2>&1 | grep -v "include " | grep -v "(INFO)" | grep -v "Directory:" | tee -a results/all_tests/summary.log

    if [ $FAILURE_COUNT -eq 0 ]; then
        if [ $WARNING_COUNT -eq 0 ]; then
            echo -e "\e[32m"
                bash $SCRIPT_DIR/banner.sh PASSED
            echo -e "\e[0m"
        else
            echo -e "\e[33m"
                bash $SCRIPT_DIR/banner.sh WARNING
            echo -e "\e[0m"
            #return 1
        fi
    else
        echo -e "\e[31m"
            bash $SCRIPT_DIR/banner.sh FAILED
        echo -e "\e[0m"
        return 1
    fi

    return 0
}

# 実行
main
exit $?
