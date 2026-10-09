#!/bin/bash

# プラットフォーム検出
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "mingw"* || "$OSTYPE" == "cygwin"* ]]; then
    IS_WINDOWS=1
else
    IS_WINDOWS=0
fi

# このスクリプトの絶対パス
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

# ワークスペースのディレクトリ
WORKSPACE_DIR=${WORKSPACE_DIR:-$(cd "$SCRIPT_DIR/../../.." && pwd)}

# make が渡す空白区切りのリストを配列へ読み、共通配置先の空白を保護する。
# 改行区切りの明示指定も受け付ける。
# 保護する接頭辞は最初に必要になったときに 1 回だけ求め、cygpath の起動を抑える。
# cygpath -w の結果は cygpath -m の区切り文字を逆斜線にしたものと同じため、-m だけを起動する。
declare -a PATH_LIST_PREFIXES=()
function parse_path_list() {
    local -n destination="$1"
    local value="$2"
    local prefix token mixed_path
    destination=()
    if [ -z "${value//[[:space:]]/}" ]; then
        return
    fi
    if [[ "$value" == *$'\n'* ]]; then
        mapfile -t destination <<< "$value"
        return
    fi
    if [ ${#PATH_LIST_PREFIXES[@]} -eq 0 ]; then
        PATH_LIST_PREFIXES=("$WORKSPACE_DIR" "$PWD")
        if [ "$IS_WINDOWS" -eq 1 ]; then
            mixed_path=$(cygpath -m "$WORKSPACE_DIR")
            PATH_LIST_PREFIXES+=("$mixed_path" "${mixed_path//\//\\}")
        fi
    fi
    for prefix in "${PATH_LIST_PREFIXES[@]}"; do
        [ -n "$prefix" ] || continue
        value=${value//"$prefix"/"${prefix// /__MAKEFW_SPACE__}"}
    done
    IFS=$' \t\n' read -r -a destination <<< "$value"
    for token in "${!destination[@]}"; do
        destination[$token]=${destination[$token]//__MAKEFW_SPACE__/ }
    done
}
declare -a TEST_SOURCE_FILES ADD_SOURCE_FILES TEST_LIBRARY_FILES
parse_path_list TEST_SOURCE_FILES "${TEST_SRCS:-}"
parse_path_list ADD_SOURCE_FILES "${ADD_SRCS:-}"
parse_path_list TEST_LIBRARY_FILES "${MAKEFW_TEST_LIBS:-}"

# ソース ファイルのエンコード指定から LANG を得る
FILES_LANG=$(bash "$WORKSPACE_DIR/framework/makefw/bin_internal/get_files_lang.sh" "$WORKSPACE_DIR")

# テスト バイナリのパス (basename `pwd` 相当)
TEST_BINARY=bin/${PWD##*/}

# スタック サイズ制限緩和
# (1) ハード リミットのスタック サイズを取得
hard_limit=$(ulimit -H -s)
# (2) ハード リミットのスタック サイズをソフト リミットに設定
ulimit -s "$hard_limit"

# 集計値のリセット
SUCCESS_COUNT=0
WARNING_COUNT=0
FAILURE_COUNT=0

# 最終結果用文字列 (テスト中は積み上げて、最後に一括出力)
test_summary=""

# tput を安全に実行するヘルパー関数
function safe_tput() {
    if [[ -t 1 && -n "$TERM" && "$TERM" != "dumb" ]]; then
        tput "$@" 2>/dev/null || true
    fi
}

# 表示用に TEST_SRCS のパスをワークスペース相対へ正規化
function format_src_path_for_display() {
    local src="$1"
    local normalized_src
    local normalized_workspace

    normalized_src=$(realpath -m "$src" 2>/dev/null || printf '%s\n' "$src")
    normalized_workspace=$(realpath -m "$WORKSPACE_DIR" 2>/dev/null || printf '%s\n' "$WORKSPACE_DIR")

    if [ $IS_WINDOWS -eq 1 ]; then
        normalized_src=$(cygpath -u "$normalized_src" 2>/dev/null || printf '%s\n' "$normalized_src")
        normalized_workspace=$(cygpath -u "$normalized_workspace" 2>/dev/null || printf '%s\n' "$normalized_workspace")
    fi

    normalized_src=${normalized_src//\\//}
    normalized_workspace=${normalized_workspace//\\//}

    case "$normalized_src" in
        "$normalized_workspace"/*)
            printf '%s\n' "${normalized_src#"$normalized_workspace"/}"
            ;;
        *)
            printf '%s\n' "$normalized_src"
            ;;
    esac
}

# md5sum の出力差異に依存せず、チェックサム値だけを取得
# Windows では _MD5_CACHE を参照し、未登録の場合は md5sum にフォールバックする
function get_md5_checksum() {
    local src="$1"

    if [ $IS_WINDOWS -eq 1 ] && declare -p _MD5_CACHE &>/dev/null && [[ -v _MD5_CACHE["$src"] ]]; then
        printf '%s\n' "${_MD5_CACHE[$src]}"
        return
    fi

    md5sum "$src" 2>/dev/null | awk 'NR == 1 { print $1 }'
}

# 引数に渡したファイル一覧の MD5 を、1 回の powershell.exe 呼び出しで一括取得し
# _MD5_CACHE へキャッシュする (Windows 専用。get_md5_checksum の高速化用)
function populate_md5_cache_windows() {
    local _ps_cmd=""
    if command -v powershell.exe &>/dev/null; then
        _ps_cmd="powershell.exe"
    elif [ -f "/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe" ]; then
        _ps_cmd="/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe"
    fi
    if [ -z "$_ps_cmd" ]; then
        return 0
    fi

    declare -gA _MD5_CACHE
    local _src_keys=()
    local ps_script=""
    local src
    for src in "$@"; do
        local win_path
        win_path=$(cygpath -w "$src" 2>/dev/null || printf '%s\n' "$src")
        ps_script+="(Get-FileHash -LiteralPath '${win_path//\'/\'\'}' -Algorithm MD5).Hash.ToLower();"
        _src_keys+=("$src")
    done
    local ps_output
    ps_output=$("$_ps_cmd" -NoProfile -Command "$ps_script" 2>/dev/null)
    local _i=0
    while IFS= read -r _line; do
        # PowerShell の出力は CRLF のため末尾の \r を除去する
        _line="${_line%$'\r'}"
        if [ -n "$_line" ] && [ $_i -lt ${#_src_keys[@]} ]; then
            _MD5_CACHE["${_src_keys[$_i]}"]="$_line"
        fi
        ((_i++))
    done <<< "$ps_output"
}

# leaf ディレクトリの再テスト スキップ判定に使うシグネチャを生成する。
# TEST_SRCS / ADD_SRCS / このディレクトリ直下の makepart.mk・makelocal.mk を対象に
# MD5 チェックサムを計算し、ソート済みの "<hash>  <相対パス>" 形式で出力する。
# 対象ファイルが1つも無い場合、またはいずれかの MD5 計算に失敗した場合は
# 何も出力せず終了コード 1 を返す (呼び出し側はスキップ判定・スタンプ書き込みの
# 両方を諦め、必ずテストを実行する)。
function compute_test_signature() {
    local -a sig_srcs=()
    local src

    for src in "${TEST_SOURCE_FILES[@]}" "${ADD_SOURCE_FILES[@]}"; do
        [ -n "$src" ] && sig_srcs+=("$src")
    done
    for src in makepart.mk makelocal.mk; do
        [ -f "$src" ] && sig_srcs+=("$src")
    done
    for src in "${TEST_LIBRARY_FILES[@]}"; do
        [ -n "$src" ] && [ -f "$src" ] && sig_srcs+=("$src")
    done
    # このディレクトリ直下で自動収集・コンパイルされる *.c/*.cc/*.cpp も対象に含める。
    # TEST_SRCS/ADD_SRCS には現れないテスト コード自体 (gtest の *Test.cc 等) の変更を
    # 検出するため。*.inject.* も対象ソースへ結合されて実際の挙動に影響するため含める。
    for src in *.c *.cc *.cpp; do
        [ -f "$src" ] && sig_srcs+=("$src")
    done

    if [ ${#sig_srcs[@]} -eq 0 ]; then
        return 1
    fi

    # 抜粋する共通ソースと相対 include のヘッダーも変更検出に含める。
    local evidence_sources
    if ! evidence_sources=$(python3 "$SCRIPT_DIR/test_subprocedures.py" \
        --list-inputs --is-windows "$IS_WINDOWS" --encoding "${FILES_LANG#*.}"); then
        return 1
    fi
    while IFS= read -r src; do
        # Windows の Python はテキスト出力の改行を CRLF にするため、末尾の \r を除去する
        src="${src%$'\r'}"
        [ -n "$src" ] && sig_srcs+=("$src")
    done <<< "$evidence_sources"

    # 集計処理を変更した場合も古いエビデンスを再利用しない。
    for src in exec_test_c_cpp.sh get_test_code_c_cpp.awk insert_summary_c_cpp.py \
        test_summary.py test_subprocedures.py gtest_summary_groups.py; do
        sig_srcs+=("$SCRIPT_DIR/$src")
    done

    local entries=""
    for src in "${sig_srcs[@]}"; do
        local checksum
        checksum=$(get_md5_checksum "$src")
        if [ -z "$checksum" ]; then
            return 1
        fi
        entries+="$checksum  $(format_src_path_for_display "$src")"$'\n'
    done

    printf '%s' "$entries" | LC_ALL=C sort
}

# テスト一覧を取得
function list_tests() {
    local -a command=("./$TEST_BINARY" --gtest_list_tests)
    if [ "${1:-}" = "all" ]; then
        command=(env -u GTEST_FILTER "./$TEST_BINARY" --gtest_list_tests "--gtest_filter=*")
    fi
    "${command[@]}" | awk '
    /^[^ ]/ {suite=$1}
    /^  / {print suite substr($0, 3)}'
    return ${PIPESTATUS[0]}
}

# テスト バイナリと各引数を個別に渡す。空白を含む配置先でも再解析しない。
function execute_test_case() {
    local test_name="$1"
    local exit_file="$2"
    local -a command=("./$TEST_BINARY" --gtest_color=yes "--gtest_filter=$test_name")
    local exit_code

    export LANG="$FILES_LANG"
    printf '%s\n' '----'
    local -a summary_options=(--test-id "$test_name" --encoding "${FILES_LANG#*.}")
    # パラメーター テストの全体サマリーは定義単位で生成済み。
    if [[ "${test_name#*.}" == */* ]]; then
        summary_options+=(--code-only)
    fi
    local -a evidence_status
    python3 "$SCRIPT_DIR/test_subprocedures.py" --language c_cpp \
        --is-windows "$IS_WINDOWS" "${summary_options[@]}"
    evidence_status=("${PIPESTATUS[@]}")
    if [[ " ${evidence_status[*]} " =~ [[:space:]][1-9][0-9]*[[:space:]] ]]; then
        printf '%s\n' '[  FAILED  ] Test evidence generation failed.'
        printf '%s\n' 1 > "$exit_file"
        return 1
    fi
    printf '%s\n' '----'
    printf './%s --gtest_filter=%s\n' "$TEST_BINARY" "$test_name"
    if [ "$IS_WINDOWS" -eq 1 ] && [ -n "$TEST_SRCS" ]; then
        command=(OpenCppCoverage.exe "${SOURCES_OPTS[@]}" --quiet
            --export_type cobertura:coverage/coverage.xml -- "${command[@]}")
    fi
    "${command[@]}" 2>&1 | grep -v 'Note: Google Test filter' | grep -v 'Your program stop with error code:'
    exit_code=${PIPESTATUS[0]}
    if [ "$IS_WINDOWS" -ne 1 ] && [ "$exit_code" -ge 128 ]; then
        local signal=$((exit_code - 128))
        printf '\n\e[31m[  FAILED  ]\e[0m Terminated by signal %s, ' "$signal"
        case "$signal" in
            6) echo 'SIGABRT: abort.' ;;
            11) echo 'SIGSEGV: segmentation fault.' ;;
            8) echo 'SIGFPE: floating-point exception.' ;;
            4) echo 'SIGILL: illegal instruction.' ;;
            *) echo 'Abnormal termination by other signal.' ;;
        esac
    elif [ "$IS_WINDOWS" -eq 1 ] && [ "$exit_code" -ne 0 ]; then
        printf '\n\e[31m[  FAILED  ]\e[0m Exit code: %s\n' "$exit_code"
    fi
    printf '%s\n' "$exit_code" > "$exit_file"
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
    # パラメーター テストの prefix をテスト クラスの後に付けた ID を生成する
    # test_name: google test で内部的に扱うテスト名 (パラメーターの prefix がテスト クラスの前に付与されているもの)
    # test_id: 人間系に見せるテスト名 (パラメーターの prefix がテスト クラス名の後、パラメーター名の前に付与されているもの)
    local test_id
    # '/' で分割して配列に格納 (awk による処理の代替)
    IFS='/' read -ra parts <<< "$test_name"
    if [[ ${#parts[@]} -eq 3 ]]; then
        test_id="${parts[1]}/${parts[0]}/${parts[2]}"
    else
        test_id="$test_name"
    fi

    # サブディレクトリを含めて gcda ファイルをクリア
    find . -name "*.gcda" -delete 2>/dev/null
    rm -rf obj/*.info gcov lcov > /dev/null

    mkdir -p "results/$test_id"
    local temp_file=$(mktemp)
    local temp_exit_code=$(mktemp)

    echo -e "\nRunning test: $test_id$test_comment_delim$test_comment on $TEST_BINARY"
    safe_tput cr
    echo -e "Running test: $test_id$test_comment_delim$test_comment on $TEST_BINARY" > "$temp_file"

    if [ $IS_WINDOWS -ne 1 ]; then
        # Linux
        execute_test_case "$test_name" "$temp_exit_code" 2>&1 | tee -a "$temp_file"
        if [ -n "$TEST_SRCS" ]; then
            # TEST_SRCS が指定されている場合のみカバレッジ計測
            # 探索範囲をテスト ディレクトリへ限定する。
            # --root だけを指定すると gcovr はワークスペース全体を走査し、
            # 他のディレクトリに残った無関係な gcda で読み取りに失敗すると、
            # そのテストの計測結果が失われる。
            # gcov の作業ディレクトリは gcovr の自動判定に委ねる。--gcov-object-directory
            # に固定値を与えると、候補がその 1 つに絞られ、サブディレクトリでコンパイル
            # したソース (サブフォルダー コンパイル構成) の作業ディレクトリと一致せず、
            # 読み取りに失敗する。
            local gcovr_error
            gcovr_error=$(gcovr --root "$WORKSPACE_DIR" . \
                --exclude-unreachable-branches \
                --exclude-throw-branches --json --output coverage/coverage.raw.json 2>&1 1> /dev/null)
            if [ ! -f coverage/coverage.raw.json ]; then
                echo -e "\e[33m[ WARNING ]\e[0m Coverage data could not be read:" | tee -a results/all_tests/summary.log
                echo "$gcovr_error" | tee -a results/all_tests/summary.log
            fi
            if [ -f coverage/coverage.raw.json ]; then
                # 大域の IFS の状態に依存せず、TEST_SRCS を空白区切りで分割する
                local -a test_src_list=("${TEST_SOURCE_FILES[@]}")
                if ! python "$SCRIPT_DIR/gcovr_json_normalize.py" \
                    coverage/coverage.raw.json coverage/coverage.json "$WORKSPACE_DIR" "${test_src_list[@]}"; then
                    echo -e "\e[31m[  FAILED  ]\e[0m gcovr_json_normalize.py rejected coverage data." | tee -a results/all_tests/summary.log
                    echo 1 > "$temp_exit_code"
                else
                    gcovr --root "$WORKSPACE_DIR" --add-tracefile coverage/coverage.json \
                        --cobertura-pretty --output coverage/coverage.xml 1> /dev/null 2>&1
                fi
            fi
        fi
    else
        # Windows
        execute_test_case "$test_name" "$temp_exit_code" 2>&1 |
            tee -a "$temp_file" | python "$SCRIPT_DIR/add_gtest_color.py"
        if [ -n "$TEST_SRCS" ]; then
            rm -f LastCoverageResults.log 1> /dev/null 2>&1
        fi
    fi

    # ファイル内容を直接読み込み (cat 相当)
    local result=$(<"$temp_exit_code")
    rm -f "$temp_exit_code"
    if [ $result -eq 0 ]; then
        if grep -qE "\[ *WARNING *\]" "$temp_file"; then
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
    cat "$temp_file" | sed -r 's/\x1b\[[0-9;]*m//g' > "results/$test_id/results.log"
    rm -f "$temp_file"

    # gcov で生成したファイルを削除する
    # Delete any existing .gcov files
    rm -rf gcov/* > /dev/null
    mkdir -p gcov

    if [ -n "$TEST_SRCS" ]; then
        # TEST_SRCS が指定されている場合のみカバレッジ情報を取得
        if [ $IS_WINDOWS -ne 1 ]; then
            # Linux
            # gcov でカバレッジ情報を取得する (サブディレクトリを含む)
            # Run gcov to collect coverage (including subdirectories)
            local base_dir=$(pwd)
            while IFS= read -r -d '' obj_dir; do
                # obj ディレクトリ内の gcda ファイルに対応するソース ファイルのカバレッジを取得
                for gcda in "$obj_dir"/*.gcda; do
                    if [ -f "$gcda" ]; then
                        # gcda ファイルからベース名を取得
                        base_name=$(basename "$gcda" .gcda)
                        # 対応する .c ソース ファイルを探す (テスト コード .cc は除外)
                        # inject 処理済みのソースはテスト実行ディレクトリ直下に生成されるため、
                        # 元ソースより先に選択する。
                        src_file=$(find . -maxdepth 1 -type f -name "${base_name}.c" 2>/dev/null | head -1)
                        if [ -z "$src_file" ]; then
                            src_file=$(find . -name "${base_name}.c" 2>/dev/null | head -1)
                        fi
                        if [ -n "$src_file" ]; then
                            # ソース ファイルのディレクトリで gcov を実行
                            src_dir=$(dirname "$src_file")
                            src_name=$(basename "$src_file")
                            abs_obj_dir=$(cd "$obj_dir" && pwd)
                            (cd "$src_dir" && gcov -o "$abs_obj_dir" "$src_name" > /dev/null 2>&1 && mv *.gcov "$base_dir/." 2>/dev/null)
                        fi
                    fi
                done
            done < <(find . -type d -name obj -print0 2>/dev/null)
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
            if [ -f coverage/coverage.xml ]; then
                python "$SCRIPT_DIR/cobertura2gcov.py" coverage/coverage.xml gcov/ 1> /dev/null 2>&1
            fi
        fi

        if ls gcov/*.gcov 1> /dev/null 2>&1; then
            for file in gcov/*.gcov; do
                cp -p "$file" "results/$test_id/${file##*/}.txt"
            done
        fi

        # 各回のテスト結果を積み上げ
        if [ $IS_WINDOWS -ne 1 ] && [ -f coverage/coverage.json ]; then
            if [ -f coverage/accumulated_coverage.json ]; then
                gcovr --root "$WORKSPACE_DIR" \
                    --add-tracefile coverage/accumulated_coverage.json \
                    --add-tracefile coverage/coverage.json \
                    --json --output coverage/merged_coverage.json 1> /dev/null 2>&1
                mv coverage/merged_coverage.json coverage/accumulated_coverage.json
            else
                cp -p coverage/coverage.json coverage/accumulated_coverage.json
            fi
        elif [ -f coverage/coverage.xml ]; then
            python "$SCRIPT_DIR/cobertura_accumulate.py" coverage/coverage.xml coverage/accumulated_coverage.xml 1> /dev/null 2>&1
        else
            echo -e "\e[33m[ WARNING ]\e[0m Coverage file was not generated: coverage/coverage.xml" | tee -a results/all_tests/summary.log
        fi
    fi

    # 各テストの coverage.xml を退避 (デバッグ用)
    #mv coverage/coverage.xml results/$test_id/.
    rm -f coverage/coverage.xml 1> /dev/null 2>&1
    rm -f coverage/coverage.json coverage/coverage.raw.json 1> /dev/null 2>&1

    return $result
}

# メイン処理
function main() {
    # サブディレクトリを含めて gcda ファイルをクリア
    find . -name "*.gcda" -delete 2>/dev/null

    # 子の設定は makefw 自身に解釈させる。引用した配置先を文字列分割しない。
    if [ ${#TEST_SOURCE_FILES[@]} -eq 0 ]; then
        local child_makefile child_dir source source_file report_file
        local make_command="${MAKEFW_SUBDIR_MAKE:-make}"
        report_file=$(mktemp)
        while IFS= read -r -d '' child_makefile; do
            child_dir=${child_makefile%/*}
            if ! MAKEFLAGS= MFLAGS= "$make_command" --no-print-directory -s \
                -C "$child_dir" -f makefile -f "$SCRIPT_DIR/query_test_sources.mk" \
                MAKEFW_BUILD=0 SUBDIRS= "MAKEFW_TEST_SRC_REPORT_FILE=$report_file" \
                __testfw_query_sources; then
                rm -f "$report_file"
                return 1
            fi
            while IFS= read -r -d '' source; do
                [ -n "$source" ] || continue
                source_file=$(realpath -m --relative-to="$PWD" "$child_dir/$source") || {
                    rm -f "$report_file"
                    return 1
                }
                TEST_SOURCE_FILES+=("$source_file")
            done < "$report_file"
        done < <(find . -mindepth 2 -type f -name makefile -print0 2>/dev/null)
        rm -f "$report_file"
        if [ ${#TEST_SOURCE_FILES[@]} -gt 0 ]; then
            mapfile -d '' -t TEST_SOURCE_FILES < <(printf '%s\0' "${TEST_SOURCE_FILES[@]}" | LC_ALL=C sort -zu)
            printf -v TEST_SRCS '%s ' "${TEST_SOURCE_FILES[@]}"
        fi
    fi

    # 再テスト スキップ判定
    # TEST_SRCS / ADD_SRCS / ローカル makepart.mk・makelocal.mk のハッシュが前回のクリーンな成功時から
    # 変わっておらず、GTEST_FILTER によるフィルター実行でもない場合は、実際のテスト実行そのものを省略する。
    # MAKEFW_TEST_FORCE=1 を指定すると、このスキップを無視して必ず実行する。
    local test_stamp_file="test.stamp"
    local test_signature_file
    test_signature_file=$(mktemp)
    local test_signature_ok=1

    if [ $IS_WINDOWS -eq 1 ]; then
        local -a signature_srcs_for_cache=()
        local ssrc
        for ssrc in "${TEST_SOURCE_FILES[@]}" "${ADD_SOURCE_FILES[@]}"; do
            [ -n "$ssrc" ] && signature_srcs_for_cache+=("$ssrc")
        done
        for ssrc in makepart.mk makelocal.mk; do
            [ -f "$ssrc" ] && signature_srcs_for_cache+=("$ssrc")
        done
        for ssrc in "${TEST_LIBRARY_FILES[@]}"; do
            [ -n "$ssrc" ] && [ -f "$ssrc" ] && signature_srcs_for_cache+=("$ssrc")
        done
        for ssrc in *.c *.cc *.cpp; do
            [ -f "$ssrc" ] && signature_srcs_for_cache+=("$ssrc")
        done
        if [ ${#signature_srcs_for_cache[@]} -gt 0 ]; then
            populate_md5_cache_windows "${signature_srcs_for_cache[@]}"
        fi
    fi

    if ! compute_test_signature > "$test_signature_file"; then
        test_signature_ok=0
        rm -f "$test_signature_file"
    fi

    if [ $test_signature_ok -eq 1 ] && [ -s "$test_signature_file" ] \
        && [ -z "${GTEST_FILTER+x}" ] && [ "${MAKEFW_TEST_FORCE:-0}" != "1" ] \
        && [ -f "$test_stamp_file" ] && cmp -s "$test_signature_file" "$test_stamp_file"; then
        echo "INFO: Skipping test (dependencies are unchanged and clean)"
        rm -f "$test_signature_file"
        return 0
    fi

    rm -rf obj/*.info gcov lcov coverage results
    mkdir coverage
    mkdir results
    mkdir -p results/all_tests

    if [ $IS_WINDOWS -eq 1 ]; then
        # Windows
        # OpenCppCoverage のソース指定オプションを生成
        SOURCES_OPTS=()
        # カレント ディレクトリの絶対パスを Windows 形式で取得 (スラッシュをバックスラッシュに変換)
        # PDB には長い形式のパスが記録されるため、8.3 形式の短い名前 (RUNNER~1 など) を含む
        # パスのままでは --sources が一致しない。cygpath -l で長い形式へ展開する。
        # see: https://www.cygwin.com/cygwin-ug-net/cygpath.html
        local current_dir=$(cygpath -l -w "$(pwd)" 2>/dev/null || pwd -W 2>/dev/null)
        current_dir=${current_dir//\//\\}
        for src in "${TEST_SOURCE_FILES[@]}"; do
            # パスからファイル名のみを抽出 (basename 相当)
            local src_basename=${src##*/}
            # Windows 形式の絶対パスに結合
            local src_fullpath="$current_dir\\$src_basename"
            SOURCES_OPTS+=(--sources "$src_fullpath")
        done
    fi

    # google test は、GTEST_FILTER が定義されている場合は空文字でもフィルターを行う
    # そのため、指定があるかどうかは環境変数の有無をチェックする必要がある
    if [[ "${GTEST_FILTER+x}" ]]; then
        echo -e "\e[33m"
            echo "Note: GTEST_FILTER = $GTEST_FILTER"
        echo -e "\e[0m"
    fi

    # テスト対象ソースの md5 を取得
    echo -e "Test start on $(export LANG=C && date)." | tee -a results/all_tests/summary.log
    echo -e "----" | tee -a results/all_tests/summary.log
    if [ -n "$TEST_SRCS" ]; then
        # TEST_SRCS が指定されている場合のみ MD5 チェックサムを表示
        # (Windows での _MD5_CACHE 一括取得は、再テスト スキップ判定のシグネチャ計算時に
        #  ADD_SRCS・ローカル makefile も含めてすでに実施済み)

        safe_tput cr
        echo -e "MD5 checksums of files in TEST_SRCS:" | tee -a results/all_tests/summary.log
        safe_tput cr
        for src in "${TEST_SOURCE_FILES[@]}"; do
            local checksum
            local display_src

            checksum=$(get_md5_checksum "$src")
            if [ -z "$checksum" ]; then
                echo -e "\e[31mError: Failed to calculate MD5: $src\e[0m" | tee -a results/all_tests/summary.log
                bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
                rm -f "$test_stamp_file" "$test_signature_file"
                return 1
            fi

            display_src=$(format_src_path_for_display "$src")
            printf '%s  %s\n' "$checksum" "$display_src" | tee -a results/all_tests/summary.log
            safe_tput cr
        done
        echo "----" | tee -a results/all_tests/summary.log
        safe_tput cr
    fi

    # TEST_BINARY の存在チェック
    if [ ! -f "$TEST_BINARY" ]; then
        echo -e "\e[31mError: Test binary not found: $TEST_BINARY\e[0m" | tee -a results/all_tests/summary.log
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        rm -f "$test_stamp_file" "$test_signature_file"
        return 1
    fi

    tests=$(list_tests)
    local list_exit_code=$?
    #tests=$(echo "$tests" | sort)
    if [ $list_exit_code -ne 0 ]; then
        echo -e "\e[31mError: Failed to execute test binary: $TEST_BINARY (exit code: $list_exit_code)\e[0m" | tee -a results/all_tests/summary.log
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        echo ""
        rm -f "$test_stamp_file" "$test_signature_file"
        return 1
    fi
    # テスト数をカウント (wc -l 相当)
    if [[ -z "$tests" ]]; then
        test_count=0
    else
        IFS=$'\n' read -d '' -r -a test_array <<< "$tests"
        test_count=${#test_array[@]}
    fi
    echo "Found $test_count test(s)."
    safe_tput cr

    # IFS を大域へ設定したままにすると、run_test の内側で $TEST_SRCS が単語分割されず、
    # 複数ソースを指定したテストのカバレッジ集計が空になる。
    # テスト名の行分割は read の一時的な IFS で行い、大域の IFS は既定のままにする。
    if [[ "${GTEST_FILTER+x}" ]]; then
        echo -e "Note: GTEST_FILTER = $GTEST_FILTER\n" >> results/all_tests/summary.log
    fi
    #echo "Test results:" >> results/all_tests/summary.log

    # フィルター適用前の一覧で PARAM を確定し、全体件数を重複掲載しない。
    local all_tests full_test_list selected_test_list group_manifest
    all_tests=$(list_tests all)
    if [ $? -ne 0 ]; then
        rm -f "$test_stamp_file" "$test_signature_file"
        return 1
    fi
    full_test_list=$(mktemp)
    selected_test_list=$(mktemp)
    group_manifest=$(mktemp)
    printf '%s\n' "$all_tests" > "$full_test_list"
    printf '%s\n' "$tests" > "$selected_test_list"
    if ! python3 "$SCRIPT_DIR/gtest_summary_groups.py" prepare "$group_manifest" \
        --full-list "$full_test_list" --selected-list "$selected_test_list" \
        --is-windows "$IS_WINDOWS" --encoding "${FILES_LANG#*.}"; then
        rm -f "$full_test_list" "$selected_test_list" "$group_manifest" "$test_stamp_file" "$test_signature_file"
        return 1
    fi
    rm -f "$full_test_list" "$selected_test_list"

    # テスト バイナリが標準入力を消費しないように、専用の記述子から読み取る。
    while IFS= read -r test_name_w_comment <&3; do
        if [ -z "$test_name_w_comment" ]; then
            continue
        fi
        run_test "$test_name_w_comment"
        # すべてのテストをやり切ったほうが使い勝手が良い
        # 失敗しない前提であれば、以下を活かしても良い
        #local result=$?
        #if [ "$result" -ne 0 ]; then
        #    return 1
        #fi
    done 3<<< "$tests"

    if ! python3 "$SCRIPT_DIR/gtest_summary_groups.py" finish "$group_manifest" --encoding "${FILES_LANG#*.}"; then
        FAILURE_COUNT=$((FAILURE_COUNT + 1))
    fi
    rm -f "$group_manifest"

    # 全体結果を出力
    printf '\n----\n%s' "$test_summary"

    filtered=""
    if [[ "${GTEST_FILTER+x}" ]]; then
        filtered=" (filtered, $GTEST_FILTER)"
    fi

    echo -e "----\nTotal tests\t$test_count\e[33m$filtered\e[0m\nPassed\t\t$SUCCESS_COUNT\nWarning(s)\t$WARNING_COUNT\nFailed\t\t$FAILURE_COUNT"
    echo -e "----\nTotal tests\t$test_count$filtered\nPassed\t\t$SUCCESS_COUNT\nWarning(s)\t$WARNING_COUNT\nFailed\t\t$FAILURE_COUNT" >> results/all_tests/summary.log

    # 再テスト スキップ判定用スタンプの更新
    # GTEST_FILTER によるフィルター実行ではなく、かつ全件失敗なし (クリーン) の場合のみ
    # スタンプを更新する。それ以外は、次回必ず再実行されるようスタンプを削除する。
    if [ $test_signature_ok -eq 1 ] && [ -s "$test_signature_file" ] \
        && [ -z "${GTEST_FILTER+x}" ] && [ $FAILURE_COUNT -eq 0 ]; then
        cp "$test_signature_file" "$test_stamp_file"
    else
        rm -f "$test_stamp_file"
    fi
    rm -f "$test_signature_file"

    if [ $IS_WINDOWS -ne 1 ] && [ -f coverage/accumulated_coverage.json ]; then
        gcovr --root "$WORKSPACE_DIR" --add-tracefile coverage/accumulated_coverage.json \
            --cobertura-pretty --output coverage/accumulated_coverage.xml 1> /dev/null 2>&1
    fi

    if [ -n "$TEST_SRCS" ] && [ -f coverage/accumulated_coverage.xml ]; then
        # TEST_SRCS が指定されている場合のみカバレッジ レポートを生成
        # 全体版 gcov の生成 (Linux でも cobertura2gcov.py を使用して出力)
        # 個別テストの gcov を残したままにせず、累積 XML から生成したファイルだけをコピーする
        rm -rf gcov/*
        python "$SCRIPT_DIR/cobertura2gcov.py" coverage/accumulated_coverage.xml gcov/ 1> /dev/null 2>&1

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
            python "$SCRIPT_DIR/cobertura2lcov.py" coverage/accumulated_coverage.xml "obj/$TEST_BINARY.info" 1> /dev/null 2>&1

            # genhtml は空のファイルを指定するとエラーを出力して終了するため
            # lcov の出力ファイルが空でないか確認してから genhtml を実行する
            # genhtml fails on empty files; verify that .info is not empty first
            if [ -s "obj/$TEST_BINARY.info" ]; then
                genhtml --function-coverage -o lcov "obj/$TEST_BINARY.info" 1> /dev/null 2>&1
            fi
        else
            # Windows
            ReportGenerator -reports:./coverage/accumulated_coverage.xml -targetdir:results/all_tests/lcov -reporttypes:Html 1> /dev/null 2>&1
        fi

        # lcov の文字コード パッチ処理
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
        python "$SCRIPT_DIR/cobertura2gcovr.py" coverage/accumulated_coverage.xml 2>&1 | tee -a results/all_tests/summary.log

        # 全体カバレッジ計測用に、カバレッジ xml を保持
        cp -p coverage/accumulated_coverage.xml results/all_tests/coverage.xml
        if [ -f coverage/accumulated_coverage.json ]; then
            cp -p coverage/accumulated_coverage.json results/all_tests/coverage.json
        fi
    elif [ -n "$TEST_SRCS" ] && [ "$test_count" -gt 0 ]; then
        echo -e "\e[33m[ WARNING ]\e[0m Accumulated coverage file was not generated: coverage/accumulated_coverage.xml" | tee -a results/all_tests/summary.log
    fi

    # Clean (サブディレクトリを含めて gcda ファイルをクリア)
    find . -name "*.gcda" -delete 2>/dev/null
    rm -rf obj/*.info gcov lcov coverage

    # Banner
    echo ""
    if [ $FAILURE_COUNT -eq 0 ]; then
        if [ $WARNING_COUNT -eq 0 ]; then
            bash "$SCRIPT_DIR/banner.sh" PASSED "\e[32m"
            echo ""
        else
            bash "$SCRIPT_DIR/banner.sh" WARNING "\e[33m"
            echo ""
            #return 1
        fi
    else
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        echo ""
        return 1
    fi

    return 0
}

# 実行
main
exit $?
