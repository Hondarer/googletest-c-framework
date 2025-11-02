#!/usr/bin/awk -f

# テスト結果ログにサマリを挿入する AWK スクリプト
# 使用方法: cat results.log | awk -f insert_summary.awk > results_with_summary.log

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
  check_count = 0;  # 確認内容のリスト項目数
}

{
  buf[NR] = $0

  # 5 種類のタグを検出
  # [状態] / [Pre-Assert手順] / [手順] / [Pre-Assert確認] / [確認]
  if (match($0, /\[状態\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") state[++s_idx] = s
  } else if (match($0, /\[手順\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") act[++act_idx] = s
  } else if (match($0, /\[Pre-Assert手順\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") pre_step[++pre_s_idx] = s
  } else if (match($0, /\[Pre-Assert確認\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      pre_chk[++pre_c_idx] = s
      if (is_list_item(s)) check_count++
    }
  } else if (match($0, /\[確認\]/)) {
    s = trim(substr($0, RSTART + RLENGTH))
    if (s != "") {
      asrt_chk[++as_c_idx] = s
      if (is_list_item(s)) check_count++
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
      # 手順がある場合のみ前改行を入れる
      if (act_idx > 0 || pre_s_idx > 0) {
        print "\n### 確認内容 (" check_count ")\n"
      } else {
        print "### 確認内容 (" check_count ")\n"
      }
      for (i=1; i<=pre_c_idx; i++) print pre_chk[i]
      for (i=1; i<=as_c_idx; i++) print asrt_chk[i]

      print "----"
    }
    print buf[n]
  }
}
