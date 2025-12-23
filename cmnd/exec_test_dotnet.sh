#!/bin/bash

# このスクリプトのパス (dirname "$0" 相当)
SCRIPT_DIR=${0%/*}

# ワークスペースのディレクトリ
WORKSPACE_FOLDER=$SCRIPT_DIR/../../

# プロジェクト名 (カレントディレクトリ名から取得)
PROJECT_NAME=$(basename "$(pwd)")

# 出力ディレクトリ
OUTPUT_DIR=${OUTPUT_DIR:-bin}
RESULTS_DIR=results

# 最終結果用変数
EXIT_CODE=0

# メイン処理
function main() {
    # 既存の results ディレクトリを削除して新規作成
    rm -rf "$RESULTS_DIR"
    mkdir -p "$RESULTS_DIR"

    echo -e "Test start on $(export LANG=C && date)." | tee "$RESULTS_DIR/summary.log"
    echo -e "Project: $PROJECT_NAME" | tee -a "$RESULTS_DIR/summary.log"
    echo -e "----" | tee -a "$RESULTS_DIR/summary.log"
    tput cr

    # dotnet test を実行してコンソールとファイルに出力
    # --no-build オプションで既にビルド済みのバイナリを使用
    # --verbosity normal でテスト結果を表示
    # xUnit の診断メッセージと誤解を招くビルドメッセージをフィルタリング
    # 連続する空白行を圧縮
    # ログファイルにはプレーンテキストで保存、コンソールには色付けして表示
    set -o pipefail
    if dotnet test -c "$CONFIG" --no-build -o "$OUTPUT_DIR" --verbosity normal 2>&1 | \
        grep -v '^\[xUnit\.net' | \
        grep -v 'にビルドを開始しました' | \
        grep -v 'Build started' | \
        grep -v 'ビルドに成功しました' | \
        grep -v 'Build succeeded' | \
        grep -v '^\s*[0-9]\+\s*個の警告' | \
        grep -v '^\s*[0-9]\+\s*Warning(s)' | \
        grep -v '^\s*[0-9]\+\s*エラー' | \
        grep -v '^\s*[0-9]\+\s*Error(s)' | \
        grep -v '経過時間' | \
        grep -v 'Time Elapsed' | \
        cat -s | \
        tee -a "$RESULTS_DIR/summary.log" | \
        sed -e 's/^  成功 /  \o033[32m成功\o033[0m /g' \
            -e 's/^  Passed /  \o033[32mPassed\o033[0m /g' \
            -e 's/^  失敗 /  \o033[31m失敗\o033[0m /g' \
            -e 's/^  Failed /  \o033[31mFailed\o033[0m /g' \
            -e 's/^  スキップ /  \o033[33mスキップ\o033[0m /g' \
            -e 's/^  Skipped /  \o033[33mSkipped\o033[0m /g'; then
        EXIT_CODE=0
    else
        EXIT_CODE=$?
    fi

    echo -e "----"
    tput cr

    # Banner 表示
    echo ""
    if [ $EXIT_CODE -eq 0 ]; then
        bash "$SCRIPT_DIR/banner.sh" PASSED "\e[32m"
        echo ""
    else
        bash "$SCRIPT_DIR/banner.sh" FAILED "\e[31m"
        echo ""
    fi

    return $EXIT_CODE
}

# 実行
main
exit $?
