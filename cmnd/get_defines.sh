#!/bin/bash

# このスクリプトのパス
SCRIPT_DIR=$(dirname "$0")

# ワークスペースのディレクトリ
WORKSPACE_FOLDER=$SCRIPT_DIR/../../

# c_cpp_properties.json のパス
c_cpp_properties="$WORKSPACE_FOLDER/.vscode/c_cpp_properties.json"

# defines の値を抽出
# 1. awk を使用して c_cpp_properties.json ファイルから defines の値を抽出
# 2. 行末のコメントを無視
# 3. 出力時の前後の不要な空白を除去
defines=$(awk -v workspace_root="$WORKSPACE_FOLDER" '
    /"defines": \[/,/\]/ {
        if ($0 ~ /"defines": \[/) { in_defines=1; next }
        if (in_defines && $0 ~ /\]/) { in_defines=0; next }
        if (in_defines) {
            sub(/\/\/.*/, "", $0) # 行末のコメントを削除
            gsub(/"|,/, "", $0)
            gsub(/^[ \t]+|[ \t]+$/, "", $0)
            if ($0 != "") print $0
        }
    }
' "$c_cpp_properties")

# 結果を出力
echo "$defines" | while IFS= read -r define; do
    echo "$define"
done
