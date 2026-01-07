#!/usr/bin/awk -f

# テスト結果ログにサマリを挿入する AWK スクリプト
# 使用方法: cat results.log | awk -f insert_summary_c_cpp.awk > results_with_summary.log

function trim(s) {
  sub(/^ /, "", s);                 # 行頭の空白 1 つだけを削除
  sub(/[ \t]+$/, "", s);            # 行末の空白群を削除
  return s
}

function is_list_item(s) {
  # 1文字目がリスト記号または番号付きリストの場合に真を返す
  if (s ~ /^-/ || s ~ /^\*/ || s ~ /^\+/ || s ~ /^[0-9]+\./) {
    return 1
  }
  return 0
}

# 各カテゴリの格納用 (遭遇順維持)
BEGIN {
  s_idx = 0; act_idx = 0; pre_s_idx = 0; pre_c_idx = 0; as_c_idx = 0;
  # 確認内容のカテゴリ別リスト項目数
  check_normal = 0;      # 正常系
  check_semi_normal = 0; # 準正常系
  check_abnormal = 0;    # 異常系
  check_unspecified = 0; # カテゴリ未指定
}

{
  buf[NR] = $0

  # タグを検出（より具体的なパターンを先にチェック）
  if (match($0, /\[状態\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") state[++s_idx] = s
  } else if (match($0, /\[手順\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") act[++act_idx] = s
  } else if (match($0, /\[Pre-Assert手順\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") pre_step[++pre_s_idx] = s
  } else if (match($0, /\[Pre-Assert確認_正常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[++pre_c_idx] = s
      if (is_list_item(s)) check_normal++
    }
  } else if (match($0, /\[確認_正常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[++as_c_idx] = s
      if (is_list_item(s)) check_normal++
    }
  } else if (match($0, /\[Pre-Assert確認_準正常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[++pre_c_idx] = s
      if (is_list_item(s)) check_semi_normal++
    }
  } else if (match($0, /\[確認_準正常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[++as_c_idx] = s
      if (is_list_item(s)) check_semi_normal++
    }
  } else if (match($0, /\[Pre-Assert確認_異常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[++pre_c_idx] = s
      if (is_list_item(s)) check_abnormal++
    }
  } else if (match($0, /\[確認_異常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[++as_c_idx] = s
      if (is_list_item(s)) check_abnormal++
    }
  } else if (match($0, /\[Pre-Assert確認\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[++pre_c_idx] = s
      if (is_list_item(s)) check_unspecified++
    }
  } else if (match($0, /\[確認\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[++as_c_idx] = s
      if (is_list_item(s)) check_unspecified++
    }
  }
}

END {
  # サマリ項目が 1 つでも存在するかチェック
  has_summary = (s_idx > 0 || act_idx > 0 || pre_s_idx > 0 || pre_c_idx > 0 || as_c_idx > 0)

  # 出力
  for (n=1; n<=NR; n++) {
    # 1 行目の直前にサマリ章を挿入 (項目がある場合のみ)
    if (n == 1 && has_summary) {
      print "## テスト項目"
      # --- 状態 ---
      print "\n### 状態\n"
      for (i=1; i<=s_idx; i++) print state[i]

      # --- 手順 (Act優先 → Pre-Assert) ---
      # 状態がある場合のみ前改行を入れる
      if (s_idx > 0) {
        print "\n### 手順\n"
      } else {
        print "### 手順\n"
      }
      for (i=1; i<=act_idx; i++) print act[i]
      for (i=1; i<=pre_s_idx; i++) print pre_step[i]

      # --- 確認内容 (Pre-Assert → Assert) ---
      # カテゴリ未指定のみかどうかを判定
      has_categorized = (check_normal > 0 || check_semi_normal > 0 || check_abnormal > 0)

      # ヘッダー文字列を生成
      if (!has_categorized) {
        # 後方互換モード: カテゴリ未指定のみの場合
        check_header = "### 確認内容 (" check_unspecified ")\n"
      } else {
        # 新フォーマット: カテゴリ別表示
        check_header = "### 確認内容 ("
        first = 1  # 最初のカテゴリかどうかのフラグ

        if (check_normal > 0) {
          if (!first) check_header = check_header ", "
          check_header = check_header "正常系:" check_normal
          first = 0
        }
        if (check_semi_normal > 0) {
          if (!first) check_header = check_header ", "
          check_header = check_header "準正常系:" check_semi_normal
          first = 0
        }
        if (check_abnormal > 0) {
          if (!first) check_header = check_header ", "
          check_header = check_header "異常系:" check_abnormal
          first = 0
        }
        if (check_unspecified > 0) {
          if (!first) check_header = check_header ", "
          check_header = check_header "カテゴリ未指定:" check_unspecified
          first = 0
        }

        check_header = check_header ")\n"
      }

      # 手順がある場合のみ前改行を入れる
      if (act_idx > 0 || pre_s_idx > 0) {
        print "\n" check_header "\n"
      } else {
        print check_header "\n"
      }
      for (i=1; i<=pre_c_idx; i++) print pre_chk[i]
      for (i=1; i<=as_c_idx; i++) print asrt_chk[i]

      print "----"
    }
    print buf[n]
  }
}
