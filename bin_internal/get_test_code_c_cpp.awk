#!/usr/bin/awk -f

# 使用方法: awk -v test_id="test_samplelogger.normal_call" -f get_test_code_c_cpp.awk test_file.cc

BEGIN {
    extracting = 0;              # テストケースの抽出中フラグ
    brace_count = 0;             # 波括弧のカウント
    buffer = "";                 # コメント用バッファ
    in_multiline_comment = 0;    # 複数行コメント中フラグ
    test_found = 0;              # テストケースを見つけたフラグ

    # PLATFORM_WINDOWS / PLATFORM_LINUX の #if 評価用スタック (depth==0 は常にアクティブ)
    # is_windows が未指定の場合は評価せず、PLATFORM_* の #if も未知条件として素通しする
    depth = 0;
    filter_platform = (is_windows != "");
    platform_windows = (filter_platform && is_windows == "1");
    platform_linux = (filter_platform && is_windows == "0");

    # test_id を "." で分割
    split(test_id, parts, "\\.");
    if (length(parts) != 2) {
        print "Error: Invalid test_id format. Use test_suite_name.test_name" > "/dev/stderr";
        exit 1;
    }

    test_suite_name = parts[1];
    # "/" が含まれている場合、"/" までを削除
    if (test_suite_name ~ /\//) {
        split(test_suite_name, temp_parts, "/");
        prefix = temp_parts[1];
        test_suite_name = temp_parts[length(temp_parts)];
    }

    # テストケース名 (パラメータテストの通番は取り除く)
    test_name = parts[2];
    # "/" が含まれている場合、"/" からを削除
    if (test_name ~ /\//) {
        split(test_name, temp_parts, "/");
        test_name = temp_parts[1];
    }
}

# プリプロセッサディレクティブ (#if, #else, #endif など) はバッファをクリアしてスキップ
/^[[:space:]]*#/ {
    if (!extracting) {
        buffer = "";
        next;
    }
}

# 複数行コメントの開始と終了が同じ行にある場合
/\/\*.*\*\// {
    if (!extracting) {
        buffer = buffer $0 "\n";
        next;
    }
}

# 複数行コメントの開始
/\/\*/ {
    if (!extracting) {
        in_multiline_comment = 1;
        buffer = buffer $0 "\n";
        next;
    }
}

# 複数行コメントの終了
/\*\// {
    if (!extracting) {
        in_multiline_comment = 0;
        buffer = buffer $0 "\n";
        next;
    }
}

# 複数行コメント中はバッファに追加
in_multiline_comment {
    if (!extracting) {
        buffer = buffer $0 "\n";
        next;
    }
}

# 行コメントをバッファに追加
/^[[:space:]]*\/\// {
    if (!extracting) {
        buffer = buffer $0 "\n";
        next;
    }
}

# 空行でバッファをクリア（複数行コメント中を除く）
/^[[:space:]]*$/ {
    if (!extracting && !in_multiline_comment && buffer != "") {
        buffer = "";  # バッファをクリア
        next;
    }
}

# } を見つけたらバッファをクリア
/^[[:space:]]*}[[:space:]]*$/ {
    if (!extracting) {
        buffer = "";
        next;
    }
}

# TEST, TEST_F, TEST_P, INSTANTIATE_TEST_SUITE_P の形式にマッチするテストの開始地点を判定
{
    # 動的な正規表現を構築
    test_pattern = "^[[:space:]]*TEST(_[FP]*)?\\([[:space:]]*" test_suite_name "[[:space:]]*,[[:space:]]*" test_name "[[:space:]]*\\)";
    test_pattern2 = "^[[:space:]]*INSTANTIATE_TEST_SUITE_P\\([[:space:]]*" prefix "[[:space:]]*,[[:space:]]*" test_suite_name "[[:space:]]*,";

    if ($0 ~ test_pattern || $0 ~ test_pattern2) {
        if ($0 ~ test_pattern)
        {
            extracting = 1;
            brace_count = 0;  # 新しいブロックのためカウントをリセット
        }
        else if ($0 ~ test_pattern2)
        {
            extracting = 2;
            brace_count = 1; # INSTANTIATE_TEST_SUITE_P に続く "("" をカウントアップしておく
        }
        test_found++;

        if (test_found > 1)
        {
            printf "\n";
        }

        # コメントがバッファリングされている場合は出力
        if (buffer != "") {
            printf "%s", buffer;
        }
        buffer = "";  # バッファをクリア

        print $0;
        next;
    }
}

# テストケース本体の中の #if/#elif/#else/#endif を評価する。
# PLATFORM_WINDOWS / PLATFORM_LINUX 単独の defined() 判定のみを対象とし、
# 複合条件や #ifdef/#ifndef 等の未知条件はこれまでどおり素通しする (フィルタしない)。
extracting && /^[[:space:]]*#/ {
    # gawk の \b は単語境界ではなくバックスペース文字を指すため、
    # ディレクティブ種別の判定には [[:space:]] または行末 ($) を用いる
    is_start = ($0 ~ /^[[:space:]]*#[[:space:]]*(ifdef|ifndef|if)([[:space:]]|$)/);
    is_elif  = ($0 ~ /^[[:space:]]*#[[:space:]]*elif([[:space:]]|$)/);
    is_else  = ($0 ~ /^[[:space:]]*#[[:space:]]*else([[:space:]]|$)/);
    is_endif = ($0 ~ /^[[:space:]]*#[[:space:]]*endif([[:space:]]|$)/);

    if (is_start) {
        parent_active = (depth == 0) ? 1 : stk_active[depth];
        depth++;
        stk_parent[depth] = parent_active;

        if (filter_platform && $0 ~ /^[[:space:]]*#[[:space:]]*if[[:space:]]+defined[[:space:]]*\([[:space:]]*PLATFORM_WINDOWS[[:space:]]*\)[[:space:]]*$/) {
            stk_known[depth] = 1;
            stk_active[depth] = parent_active && platform_windows;
            stk_taken[depth] = stk_active[depth];
        } else if (filter_platform && $0 ~ /^[[:space:]]*#[[:space:]]*if[[:space:]]+defined[[:space:]]*\([[:space:]]*PLATFORM_LINUX[[:space:]]*\)[[:space:]]*$/) {
            stk_known[depth] = 1;
            stk_active[depth] = parent_active && platform_linux;
            stk_taken[depth] = stk_active[depth];
        } else {
            # 未知条件: フィルタせず、現在の有効状態をそのまま引き継ぐ
            stk_known[depth] = 0;
            stk_active[depth] = parent_active;
        }

        if (!stk_known[depth] && stk_active[depth]) {
            print $0;
        }
        next;
    }

    if (depth == 0) {
        # 対応する #if がない #elif/#else/#endif (通常は発生しない) は素通しする
        print $0;
        next;
    }

    if (is_elif) {
        if (stk_known[depth]) {
            if ($0 ~ /^[[:space:]]*#[[:space:]]*elif[[:space:]]+defined[[:space:]]*\([[:space:]]*PLATFORM_WINDOWS[[:space:]]*\)[[:space:]]*$/) {
                cond = platform_windows;
            } else if ($0 ~ /^[[:space:]]*#[[:space:]]*elif[[:space:]]+defined[[:space:]]*\([[:space:]]*PLATFORM_LINUX[[:space:]]*\)[[:space:]]*$/) {
                cond = platform_linux;
            } else {
                # 認識できない #elif: この分岐グループ全体を未知条件へ切り替える
                stk_known[depth] = 0;
                stk_active[depth] = stk_parent[depth];
                if (stk_active[depth]) {
                    print $0;
                }
                next;
            }
            stk_active[depth] = stk_parent[depth] && !stk_taken[depth] && cond;
            if (stk_active[depth]) {
                stk_taken[depth] = 1;
            }
        } else {
            stk_active[depth] = stk_parent[depth];
            if (stk_active[depth]) {
                print $0;
            }
        }
        next;
    }

    if (is_else) {
        if (stk_known[depth]) {
            stk_active[depth] = stk_parent[depth] && !stk_taken[depth];
            stk_taken[depth] = 1;
        } else {
            stk_active[depth] = stk_parent[depth];
            if (stk_active[depth]) {
                print $0;
            }
        }
        next;
    }

    if (is_endif) {
        if (!stk_known[depth] && stk_active[depth]) {
            print $0;
        }
        depth--;
        next;
    }

    # if/elif/else/endif 以外のプリプロセッサ行 (#define 等)
    if (depth == 0 || stk_active[depth]) {
        print $0;
    }
    next;
}

# テストケースの中身を出力
extracting {
    active = (depth == 0) ? 1 : stk_active[depth];

    if (active) {
        print $0;
    }

    if (extracting == 1)
    {
        if (active) {
            # { の数を増加
            brace_count += gsub(/\{/, "{");

            # } の数を減少
            brace_count -= gsub(/\}/, "}");
        }
    }
    else if (extracting == 2)
    {
        if (active) {
            # ( の数を増加
            brace_count += gsub(/\(/, "(");

            # ) の数を減少
            brace_count -= gsub(/\)/, ")");
        }
    }

    # ブロック終了を検知
    if (brace_count <= 0) {
        extracting = 0;
    }
}

# 他のコードブロックは無視
!extracting {
    next;
}

# END ブロックでテストが見つからなかった場合の処理
END {
    if (!test_found) {
        print "Error: Test case \"" test_id "\" not found." > "/dev/stderr";
        exit 1;  # 異常終了
    }
}
