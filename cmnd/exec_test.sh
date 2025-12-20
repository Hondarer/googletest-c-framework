#!/bin/bash

# プラットフォーム検出
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "mingw"* ]]; then
    IS_WINDOWS=1
else
    IS_WINDOWS=0
fi

# このスクリプトのパス (dirname "$0" 相当)
SCRIPT_DIR=${0%/*}

# ワークスペースのディレクトリ
WORKSPACE_FOLDER=$SCRIPT_DIR/../../

# ソースファイルのエンコード指定から LANG を得る
FILES_LANG=$(sh "$WORKSPACE_FOLDER/makefw/cmnd/get_files_lang.sh" "$WORKSPACE_FOLDER")

# テストバイナリのパス (basename `pwd` 相当)
TEST_BINARY=${PWD##*/}

# スタックサイズ制限緩和
# (1) ハードリミットのスタックサイズを取得
hard_limit=$(ulimit -H -s)
# (2) ハードリミットのスタックサイズをソフトリミットに設定
ulimit -s "$hard_limit"

# 集計値のリセット
SUCCESS_COUNT=0
WARNING_COUNT=0
FAILURE_COUNT=0

# 最終結果用文字列 (テスト中は積み上げて、最後に一括出力)
test_summary=""

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
    # 最初のスペースより前を取得 (cut -d' ' -f1 相当)
    local test_name=${1%% *}

    # 階層構造の管理上の都合で
    # パラメータテストの prefix をテストクラスの後に付けた ID を生成する
    # test_name: google test で内部的に扱うテスト名 (パラメータの prefix がテストクラスの前に付与されているもの)
    # test_id: 人間系に見せるテスト名 (パラメータの prefix がテストクラス名の後、パラメータ名の前に付与されているもの)
    local test_id
    # '/' で分割して配列に格納 (awk による処理の代替)
    IFS='/' read -ra parts <<< "$test_name"
    if [[ ${#parts[@]} -eq 3 ]]; then
        test_id="${parts[1]}/${parts[0]}/${parts[2]}"
    else
        test_id="$test_name"
    fi

    rm -rf obj/*.gcda obj/*.info gcov lcov > /dev/null

    mkdir -p results/$test_id
    local temp_file=$(mktemp)
    local temp_exit_code=$(mktemp)

    echo -e "\nRunning test: $test_id$test_comment_delim$test_comment on $TEST_BINARY"
    tput cr
    echo -e "Running test: $test_id$test_comment_delim$test_comment on $TEST_BINARY" > $temp_file

    # テストコードに着色する場合:
    # cat *.cc *.cpp 2>/dev/null | awk -v test_name=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk | awk -f $SCRIPT_DIR/insert_summary.awk | source-highlight -s cpp -f esc;

    if [ $IS_WINDOWS -ne 1 ]; then
        # Linux
        LANG=$FILES_LANG bash -c \
           "echo \"----\"; \
            cat *.cc *.cpp 2>/dev/null | awk -v test_id=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk | awk -f $SCRIPT_DIR/insert_summary.awk; \
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
            echo \$exit_code > $temp_exit_code" 2>&1 | tee -a $temp_file
        if [ -n "$TEST_SRCS" ]; then
            # TEST_SRCS が指定されている場合のみカバレッジ計測
            gcovr --exclude-unreachable-branches --cobertura-pretty --output coverage/coverage.xml 1> /dev/null 2>&1
        fi
    else
        # Windows
        if [ -n "$TEST_SRCS" ]; then
            # TEST_SRCS が指定されている場合のみカバレッジ計測
            LANG=$FILES_LANG bash -c \
               "echo \"----\"; \
                cat *.cc *.cpp 2>/dev/null | awk -v test_id=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk | awk -f $SCRIPT_DIR/insert_summary.awk; \
                echo \"----\"; \
                echo OpenCppCoverage.exe $SOURCES_OPTS --quiet --export_type cobertura:coverage/coverage.xml -- ./$TEST_BINARY --gtest_filter=\"$test_name\"; \
                OpenCppCoverage.exe $SOURCES_OPTS --quiet --export_type cobertura:coverage/coverage.xml -- ./$TEST_BINARY --gtest_color=yes --gtest_filter=\"$test_name\" 2>&1 | grep -v \"Note: Google Test filter\" | grep -v \"Your program stop with error code:\"; \
                exit_code=\${PIPESTATUS[0]}; \
                if [ \$exit_code -ne 0 ]; then \
                    echo -e \"\\n\\e[31m[  FAILED  ]\\e[0m Exit code: \$exit_code\"; \
                fi; \
                echo \$exit_code > $temp_exit_code" 2>&1 | tee -a $temp_file | python $SCRIPT_DIR/add_gtest_color.py
            rm -f LastCoverageResults.log 1> /dev/null 2>&1
        else
            # TEST_SRCS が未指定の場合はカバレッジ計測なし
            LANG=$FILES_LANG bash -c \
               "echo \"----\"; \
                cat *.cc *.cpp 2>/dev/null | awk -v test_id=\"$test_name\" -f $SCRIPT_DIR/get_test_code.awk | awk -f $SCRIPT_DIR/insert_summary.awk; \
                echo \"----\"; \
                echo ./$TEST_BINARY --gtest_filter=\"$test_name\"; \
                ./$TEST_BINARY --gtest_color=yes --gtest_filter=\"$test_name\" 2>&1 | grep -v \"Note: Google Test filter\"; \
                exit_code=\${PIPESTATUS[0]}; \
                if [ \$exit_code -ne 0 ]; then \
                    echo -e \"\\n\\e[31m[  FAILED  ]\\e[0m Exit code: \$exit_code\"; \
                fi; \
                echo \$exit_code > $temp_exit_code" 2>&1 | tee -a $temp_file | python $SCRIPT_DIR/add_gtest_color.py
        fi
    fi

    # ファイル内容を直接読み込み (cat 相当)
    local result=$(<"$temp_exit_code")
    rm -f $temp_exit_code
    if [ $result -eq 0 ]; then
        if grep -q "WARNING" $temp_file; then
            #                echo -e "$test_id\t\e[33mWARNING\e[0m\t$test_comment"
            test_summary+="$(echo -e "$test_id\t\e[33mWARNING\e[0m\t$test_comment")"$'\n'
            echo -e "$test_id\tWARNING\t$test_comment" >> results/all_tests/summary.log
            WARNING_COUNT=$((WARNING_COUNT + 1))
        else
            #                echo -e "$test_id\t\e[32mPASSED\e[0m\t$test_comment"
            test_summary+="$(echo -e "$test_id\t\e[32mPASSED\e[0m\t$test_comment")"$'\n'
            echo -e "$test_id\tPASSED\t$test_comment" >> results/all_tests/summary.log
            SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
        fi
    else
        #                echo -e "$test_id\t\e[31mFAILED\e[0m\t$test_comment"
        test_summary+="$(echo -e "$test_id\t\e[31mFAILED\e[0m\t$test_comment")"$'\n'
        echo -e "$test_id\tFAILED\t$test_comment" >> results/all_tests/summary.log
        FAILURE_COUNT=$((FAILURE_COUNT + 1))
    fi
    cat $temp_file | sed -r 's/\x1b\[[0-9;]*m//g' > results/$test_id/results.log
    rm -f $temp_file

    # gcov で生成したファイルを削除する
    # Delete any existing .gcov files
    rm -rf gcov/* > /dev/null
    mkdir -p gcov

    if [ -n "$TEST_SRCS" ]; then
        # TEST_SRCS が指定されている場合のみカバレッジ情報を取得
        if [ $IS_WINDOWS -ne 1 ]; then
            # Linux
            # gcov でカバレッジ情報を取得する
            # Run gcov to collect coverage
            gcov $TEST_SRCS -o obj > /dev/null
            # カバレッジ未通過の *.gcov ファイルは削除する
            # Delete *.gcov files without coverage
            if [ -n "`ls *.gcov 2>/dev/null`" ]; then
                for file in *.gcov; do
                    if ! grep -qE '^\s*[0-9]+\*?:' "$file"; then
                        rm "$file";
                    fi;
                done
            fi
            mv *.gcov gcov/. 1> /dev/null 2>&1
        else
            # Windows
            python $SCRIPT_DIR/cobertura2gcov.py coverage/coverage.xml gcov/ 1> /dev/null 2>&1
        fi

        if ls gcov/*.gcov 1> /dev/null 2>&1; then
            for file in gcov/*.gcov; do
                cp -p "$file" "results/$test_id/${file##*/}.txt"
            done
        fi

        # 各回のテスト結果を積み上げ
        python $SCRIPT_DIR/cobertura_accumulate.py coverage/coverage.xml coverage/accumulated_coverage.xml 1> /dev/null 2>&1
    fi

    # 各テストの coverage.xml を退避 (デバッグ用)
    #mv coverage/coverage.xml results/$test_id/.
    rm -f coverage/coverage.xml 1> /dev/null 2>&1

    return $result
}

# メイン処理
function main() {
    rm -rf obj/*.gcda obj/*.info gcov lcov coverage results
    mkdir coverage
    mkdir results
    mkdir -p results/all_tests

    if [ $IS_WINDOWS -eq 1 ]; then
        # Windows
        # OpenCppCoverage のソース指定オプションを生成
        SOURCES_OPTS=""
        # カレントディレクトリの絶対パスを Windows 形式で取得 (スラッシュをバックスラッシュに変換)
        local current_dir=$(pwd -W 2>/dev/null || cygpath -w "$(pwd)")
        current_dir=${current_dir//\//\\}
        for src in $TEST_SRCS; do
            # パスからファイル名のみを抽出 (basename 相当)
            local src_basename=${src##*/}
            # Windows 形式の絶対パスに結合
            local src_fullpath="$current_dir\\$src_basename"
            SOURCES_OPTS="$SOURCES_OPTS --sources \"$src_fullpath\""
        done
    fi

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
    if [ -n "$TEST_SRCS" ]; then
        # TEST_SRCS が指定されている場合のみ MD5 チェックサムを表示
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
    fi

    tests=$(list_tests)
    #tests=$(echo "$tests" | sort)
    # テスト数をカウント (wc -l 相当)
    if [[ -z "$tests" ]]; then
        test_count=0
    else
        IFS=$'\n' read -d '' -r -a test_array <<< "$tests"
        test_count=${#test_array[@]}
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

    # 全体結果を出力
    if [[ "${GTEST_FILTER+x}" ]]; then
        echo -e "Note: GTEST_FILTER = $GTEST_FILTER\n" >> results/all_tests/summary.log
    fi
    echo "Test results:" >> results/all_tests/summary.log
    printf '\n%s' "$test_summary"

    filtered=""
    if [[ "${GTEST_FILTER+x}" ]]; then
        filtered=" (filtered, $GTEST_FILTER)"
    fi

    echo -e "----\nTotal tests\t$test_count\e[33m$filtered\e[0m\nPassed\t\t$SUCCESS_COUNT\nWarning(s)\t$WARNING_COUNT\nFailed\t\t$FAILURE_COUNT"
    echo -e "----\nTotal tests\t$test_count$filtered\nPassed\t\t$SUCCESS_COUNT\nWarning(s)\t$WARNING_COUNT\nFailed\t\t$FAILURE_COUNT" >> results/all_tests/summary.log

    if [ -n "$TEST_SRCS" ]; then
        # TEST_SRCS が指定されている場合のみカバレッジレポートを生成
        # 全体版 gcov の生成 (Linux でも cobertura2gcov.py を使用して出力)
        python $SCRIPT_DIR/cobertura2gcov.py coverage/accumulated_coverage.xml gcov/ 1> /dev/null 2>&1

        if ls gcov/*.gcov 1> /dev/null 2>&1; then
            for file in gcov/*.gcov; do
                cp -p "$file" "results/all_tests/${file##*/}.txt"
            done
        fi

        if [ $IS_WINDOWS -ne 1 ]; then
            # Linux

            # lcov で生成したファイルを削除する
            # Delete any existing info files generated by lcov
            rm -rf obj/*.info lcov/*
            mkdir -p lcov

            # coverage/accumulated_coverage.xml をもとに、lcov の出力と互換性がある .info を生成する
            python $SCRIPT_DIR/cobertura2lcov.py coverage/accumulated_coverage.xml obj/$TEST_BINARY.info 1> /dev/null 2>&1

            # genhtml は空のファイルを指定するとエラーを出力して終了するため
            # lcov の出力ファイルが空でないか確認してから genhtml を実行する
            # genhtml fails on empty files; verify that .info is not empty first
            if [ -s obj/$TEST_BINARY.info ]; then
                genhtml --function-coverage -o lcov obj/$TEST_BINARY.info 1> /dev/null 2>&1
            fi
        else
            # Windows
            ReportGenerator -reports:./coverage/accumulated_coverage.xml -targetdir:results/all_tests/lcov -reporttypes:Html 1> /dev/null 2>&1
        fi

        # lcov の文字コードパッチ処理
        if [ $IS_WINDOWS -ne 1 ]; then
            # Linux
            if ls lcov/* 1> /dev/null 2>&1; then
                cp -rp lcov results/all_tests/.

                # FILES_LANG が utf-8 でない場合の処理
                if [[ ! "${FILES_LANG}" =~ [Uu][Tt][Ff][-+_]*8 ]]; then
                    find results/all_tests/lcov -name "*.gcov.html" | while read -r file; do
                        sed -i "s/charset=UTF-8/charset=${FILES_LANG#*.}/" "$file"
                    done
                fi
            fi
        fi

        echo "" | tee -a results/all_tests/summary.log

        # Code Coverage Report
        python $SCRIPT_DIR/cobertura2gcovr.py coverage/accumulated_coverage.xml 2>&1 | tee -a results/all_tests/summary.log

        # 全体カバレッジ計測用に、カバレッジ xml を保持
        cp -p coverage/accumulated_coverage.xml results/all_tests/coverage.xml
    fi

    # Clean
    rm -rf obj/*.gcda obj/*.info gcov lcov coverage

    # Banner
    echo ""
    if [ $FAILURE_COUNT -eq 0 ]; then
        if [ $WARNING_COUNT -eq 0 ]; then
            bash $SCRIPT_DIR/banner.sh PASSED "\e[32m"
            echo ""
        else
            bash $SCRIPT_DIR/banner.sh WARNING "\e[33m"
            echo ""
            #return 1
        fi
    else
        bash $SCRIPT_DIR/banner.sh FAILED "\e[31m"
        echo ""
        return 1
    fi

    return 0
}

# 実行
main
exit $?
