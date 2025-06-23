#!/bin/bash

# このスクリプトのパス
SCRIPT_DIR=$(dirname "$0")

# ワークスペースのディレクトリ
WORKSPACE_FOLDER=$SCRIPT_DIR/../../

# c_cpp_properties.json のパス
c_cpp_properties="$WORKSPACE_FOLDER/.vscode/c_cpp_properties.json"

# Linux 設定の defines の値を抽出
# 1. awk を使用して c_cpp_properties.json ファイルから Linux 設定の defines の値を抽出
# 2. 行末のコメントを無視
# 3. 出力時の前後の不要な空白を除去
defines=$(awk -v workspace_root="$WORKSPACE_FOLDER" '
    BEGIN { in_linux=0; in_defines=0 }
    
    # "name": "Linux" を見つけたら、Linux 設定の開始
    /"name":[[:space:]]*"Linux"/ {
        in_linux=1
        next
    }
    
    # 新しい設定が始まったら、Linux 設定を終了
    in_linux && /"name":[[:space:]]*"[^"]+"/ && !/"name":[[:space:]]*"Linux"/ {
        in_linux=0
        in_defines=0
        next
    }
    
    # Linux 設定内で defines を見つけたら
    in_linux && /"defines":[[:space:]]*\[/ {
        in_defines=1
        # 同じ行に配列の内容がある場合の処理
        if (match($0, /\[.*\]/)) {
            str = substr($0, RSTART+1, RLENGTH-2)
            # カンマで分割
            split(str, items, ",")
            for (i in items) {
                sub(/\/\/.*/, "", items[i]) # コメント削除
                gsub(/"|^\[|\]$/, "", items[i])
                gsub(/^[[:space:]]+|[[:space:]]+$/, "", items[i])
                if (items[i] != "") print items[i]
            }
            in_defines=0
        }
        next
    }
    
    # defines 配列の終了
    in_defines && /\]/ {
        in_defines=0
        next
    }
    
    # defines 配列内の値を処理
    in_defines {
        sub(/\/\/.*/, "", $0) # 行末のコメントを削除
        gsub(/"|,/, "", $0)
        gsub(/^[[:space:]]+|[[:space:]]+$/, "", $0)
        if ($0 != "") print $0
    }
    
    # configurations の終了で Linux 設定も終了
    /^[[:space:]]*\}[[:space:]]*$/ && in_linux {
        in_linux=0
        in_defines=0
    }
' "$c_cpp_properties")

# 結果を出力
echo "$defines" | while IFS= read -r define; do
    echo "$define"
done
