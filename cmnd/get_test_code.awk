#!/usr/bin/awk -f

# 使用方法: awk -v test_id="test_samplelogger.normal_call" -f get_test_code.awk test_file.cc

BEGIN {
    extracting = 0;              # テストケースの抽出中フラグ
    brace_count = 0;             # 波括弧のカウント
    buffer = "";                 # コメント用バッファ
    in_multiline_comment = 0;    # 複数行コメント中フラグ
    test_found = 0;              # テストケースを見つけたフラグ

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
    test_pattern2 = "^[[:space:]]*INSTANTIATE_TEST_SUITE_P\\([[:space:]]*" prefix "[[:space:]]*,[[:space:]]*" test_suite_name "[[:space:]]*\\,";

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

# テストケースの中身を出力
extracting {
    print $0;

    if (extracting == 1)
    {
        # { の数を増加
        brace_count += gsub(/\{/, "{");

        # } の数を減少
        brace_count -= gsub(/\}/, "}");
    }
    else if (extracting == 2)
    {
        # ( の数を増加
        brace_count += gsub(/\(/, "(");

        # ) の数を減少
        brace_count -= gsub(/\)/, ")");
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
