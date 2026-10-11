#!/bin/bash

# 実施日時を "YYYY-MM-DD hh:mm:ss <タイムゾーン>" の形式で出力する。
# 引数は date へそのまま渡す (例: -d 2026-10-11T00:59:59Z)。
#
# Cygwin 系の date (Git for Windows、MSYS2) は、TZ が未設定のとき GetTimeZoneInformation の
# StandardName から英大文字だけを取り出して略称を作り、3 文字未満なら空白 3 文字を使う。
# 日本語の Windows では %Z が空白だけになるため、その場合は UTC からの差 (%z、例: +0900) を出力する。
# UTC からの差は Bias と StandardBias から求まるため、日本語の Windows でも正しい。
# see: https://github.com/cygwin/cygwin/blob/main/winsup/cygwin/tzcode/localtime_wrapper.c

zone=$(date "$@" '+%Z') || exit
if [ -z "${zone//[[:space:]]/}" ]; then
    date "$@" '+%Y-%m-%d %H:%M:%S %z'
else
    date "$@" '+%Y-%m-%d %H:%M:%S %Z'
fi
