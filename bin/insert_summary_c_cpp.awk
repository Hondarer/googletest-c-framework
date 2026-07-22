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

# 構造コメント (// Arrange / // Pre-Assert / // Act / // Assert) のサイクル番号を返す。
# 番号なしは 1、該当しない行 (// Cleanup を含む) は -1 を返す。
function phase_cycle_number(line,   numstr) {
  if (line !~ /^[[:space:]]*\/\/[[:space:]]*(Arrange|Pre-Assert|Act|Assert)(_[0-9]+)?[[:space:]]*$/) {
    return -1
  }
  if (match(line, /_[0-9]+/)) {
    numstr = substr(line, RSTART + 1, RLENGTH - 1)
    return numstr + 0
  }
  return 1
}

# 1 サイクル分のセクション (状態 / 手順 / 確認内容) を出力する。
# multi が真のとき、見出しに "_cycle" を付与する。
function print_cycle(cycle, multi,   suffix, check_header, first, has_step) {
  suffix = multi ? ("_" cycle) : ""

  # --- 状態 ---
  print "\n### 状態" suffix
  if (s_idx[cycle] > 0) {
    print ""
    for (i = 1; i <= s_idx[cycle]; i++) print state[cycle, i]
  }

  # --- 手順 (Act優先 → Pre-Assert) ---
  print "\n### 手順" suffix "\n"
  for (i = 1; i <= act_idx[cycle]; i++) print act[cycle, i]
  for (i = 1; i <= pre_s_idx[cycle]; i++) print pre_step[cycle, i]

  # --- 確認内容 (Pre-Assert → Assert) ---
  has_step = (act_idx[cycle] > 0 || pre_s_idx[cycle] > 0)
  # カテゴリ未指定のみかどうかを判定
  has_categorized = (check_normal[cycle] > 0 || check_abnormal[cycle] > 0)

  # ヘッダー文字列を生成
  if (!has_categorized) {
    # カテゴリ未指定の件数のみを表示
    check_header = "### 確認内容" suffix " (" (check_unspecified[cycle] + 0) ")\n"
  } else {
    # 正常系 / 異常系 / カテゴリ未指定をカテゴリ別に表示
    check_header = "### 確認内容" suffix " ("
    first = 1  # 最初のカテゴリかどうかのフラグ

    if (check_normal[cycle] > 0) {
      if (!first) check_header = check_header ", "
      check_header = check_header "正常系:" check_normal[cycle]
      first = 0
    }
    if (check_abnormal[cycle] > 0) {
      if (!first) check_header = check_header ", "
      check_header = check_header "異常系:" check_abnormal[cycle]
      first = 0
    }
    if (check_unspecified[cycle] > 0) {
      if (!first) check_header = check_header ", "
      check_header = check_header "カテゴリ未指定:" check_unspecified[cycle]
      first = 0
    }

    check_header = check_header ")\n"
  }

  if (has_step) {
    print "\n" check_header
  } else {
    print check_header
  }
  for (i = 1; i <= pre_c_idx[cycle]; i++) print pre_chk[cycle, i]
  for (i = 1; i <= as_c_idx[cycle]; i++) print asrt_chk[cycle, i]
}

BEGIN {
  # サイクル管理 (// Arrange / // Pre-Assert / // Act / // Assert の番号増加で境界を検出)
  cur_cycle = 1; max_cycle = 1;
  # サマリ項目が 1 つでも存在するか
  any_content = 0;
  # TEST_F 直前の // コメント行 (description)
  desc_idx = 0; found_test = 0;
}

{
  buf[NR] = $0

  # TEST_F 前の // コメントを description として収集
  if (!found_test) {
    if ($0 ~ /^[[:space:]]*(TEST(_[FP]*)?\s*\(|TEST\s*\()/) {
      found_test = 1
    } else if ($0 ~ /^[[:space:]]*\/\//) {
      s = $0
      sub(/^[[:space:]]*\/\/[[:space:]]*/, "", s)
      sub(/[[:space:]]+$/, "", s)
      if (s != "") desc[++desc_idx] = s
    } else if ($0 !~ /^[[:space:]]*$/) {
      desc_idx = 0; delete desc  # // でも空行でもない行でリセット
    }
  }

  # サイクル境界の判定 (番号がそれまでの最大値を超えたときだけ更新する)
  ph_num = phase_cycle_number($0)
  if (ph_num > cur_cycle) {
    cur_cycle = ph_num
  }
  if (cur_cycle > max_cycle) max_cycle = cur_cycle

  # タグを検出（より具体的なパターンを先にチェック）
  if (match($0, /\[状態\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") { state[cur_cycle, ++s_idx[cur_cycle]] = s; any_content = 1 }
  } else if (match($0, /\[手順\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") { act[cur_cycle, ++act_idx[cur_cycle]] = s; any_content = 1 }
  } else if (match($0, /\[Pre-Assert手順\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") { pre_step[cur_cycle, ++pre_s_idx[cur_cycle]] = s; any_content = 1 }
  } else if (match($0, /\[Pre-Assert確認_正常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[cur_cycle, ++pre_c_idx[cur_cycle]] = s
      any_content = 1
      if (is_list_item(s)) check_normal[cur_cycle]++
    }
  } else if (match($0, /\[確認_正常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[cur_cycle, ++as_c_idx[cur_cycle]] = s
      any_content = 1
      if (is_list_item(s)) check_normal[cur_cycle]++
    }
  } else if (match($0, /\[Pre-Assert確認_異常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[cur_cycle, ++pre_c_idx[cur_cycle]] = s
      any_content = 1
      if (is_list_item(s)) check_abnormal[cur_cycle]++
    }
  } else if (match($0, /\[確認_異常系\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[cur_cycle, ++as_c_idx[cur_cycle]] = s
      any_content = 1
      if (is_list_item(s)) check_abnormal[cur_cycle]++
    }
  } else if (match($0, /\[Pre-Assert確認\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[cur_cycle, ++pre_c_idx[cur_cycle]] = s
      any_content = 1
      if (is_list_item(s)) check_unspecified[cur_cycle]++
    }
  } else if (match($0, /\[確認\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[cur_cycle, ++as_c_idx[cur_cycle]] = s
      any_content = 1
      if (is_list_item(s)) check_unspecified[cur_cycle]++
    }
  }
}

END {
  # サマリ項目が 1 つでも存在するかチェック
  has_summary = (desc_idx > 0 || any_content)

  # 出力
  for (n=1; n<=NR; n++) {
    # 1 行目の直前にサマリ章を挿入 (項目がある場合のみ)
    if (n == 1 && has_summary) {
      print "## テスト項目"
      # --- description (TEST_F 直前の // コメント) ---
      if (desc_idx > 0) {
        print ""
        for (i=1; i<=desc_idx; i++) print desc[i]
      }

      if (max_cycle <= 1) {
        print_cycle(1, 0)
      } else {
        for (c = 1; c <= max_cycle; c++) print_cycle(c, 1)
      }

      print "----"
    }
    print buf[n]
  }
}
