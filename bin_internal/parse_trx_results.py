#!/usr/bin/env python3
"""
TRX (Visual Studio Test Results) XML パーサー

dotnet test --logger "trx" が出力する TRX ファイルを解析し、
テストごとの結果を TSV で標準出力に出力する。

使用方法:
    parse_trx_results.py <trx_file>

Output:
    ClassName.MethodName<TAB>Passed|Failed (1行1テスト)
    --with-counts 指定時はレコード数と Complete|Partial を追加する。
    レコード別結果と確定できない場合、レコード数は - とする。
    --with-start-time 指定時は最も早い startTime のローカル日時を追加する。
    startTime がない場合は --fallback-start-time の値を使う。

パラメータ付きテスト (Theory) は同一メソッド名でグループ化し、
1件でも Failed があれば Failed とする。
"""

import argparse
import sys
import subprocess
from decimal import Decimal
import xml.etree.ElementTree as ET

TRX_NS = "http://microsoft.com/schemas/VisualStudio/TeamTest/2010"


def ns(tag):
    return f"{{{TRX_NS}}}{tag}"


def parse_trx(trx_path, with_counts=False, with_start_time=False, fallback_start_time=""):
    tree = ET.parse(trx_path)
    root = tree.getroot()

    # testId -> (className, methodName) のマッピングを構築
    test_id_map = {}
    expanded_definitions = {}
    test_definitions = root.find(ns("TestDefinitions"))
    if test_definitions is not None:
        for unit_test in test_definitions.findall(ns("UnitTest")):
            test_id = unit_test.get("id")
            test_method = unit_test.find(ns("TestMethod"))
            if test_method is not None:
                class_name = test_method.get("className", "")
                method_name = test_method.get("name", "")
                # className は "CalcLib.Tests.CalcLibraryTests" のような完全修飾名
                # 最後の部分だけ取得
                short_class = class_name.rsplit(".", 1)[-1] if class_name else ""
                test_id_map[test_id] = (short_class, method_name)
                expanded_definitions[test_id] = "(" in unit_test.get("name", "") or "(" in method_name

    # testId -> outcome のマッピングを構築
    results_map = {}
    expanded_results = {}
    starts = {}
    results_elem = root.find(ns("Results"))
    if results_elem is not None:
        seen_executions = set()
        for result in results_elem.iter(ns("UnitTestResult")):
            # データ駆動テストの親結果とレコード別の子結果を二重計上しない。
            if result.find(f".//{ns('UnitTestResult')}") is not None:
                continue
            execution_id = result.get("executionId")
            if execution_id and execution_id in seen_executions:
                continue
            if execution_id:
                seen_executions.add(execution_id)
            test_id = result.get("testId")
            if with_start_time and result.get("startTime"):
                value = result.get("startTime")
                instant = subprocess.check_output(["date", "-d", value, "+%s.%N"], text=True, encoding="utf-8").strip()
                starts.setdefault(test_id, []).append((Decimal(instant), value))
            outcome = result.get("outcome", "NotExecuted")
            expanded_results.setdefault(test_id, []).append(
                expanded_definitions.get(test_id, False) or "(" in result.get("testName", "")
                or result.get("dataRowInfo") is not None)
            if test_id in results_map:
                results_map[test_id].append(outcome)
            else:
                results_map[test_id] = [outcome]

    # メソッド単位でグループ化 (パラメーター付きテスト対応)
    # key: "ClassName.MethodName", value: list of outcomes
    method_outcomes = {}
    method_expanded = {}
    method_starts = {}
    for test_id, outcomes in results_map.items():
        if test_id in test_id_map:
            short_class, method_name = test_id_map[test_id]
            # パラメーター付きテストのメソッド名からパラメーター部分を除去
            # 例: "Add_ShouldReturnCorrectResult(a: 10, b: 20, expected: 30)"
            #  -> "Add_ShouldReturnCorrectResult"
            base_method = method_name.split("(")[0]
            key = f"{short_class}.{base_method}"
            if key not in method_outcomes:
                method_outcomes[key] = []
            method_outcomes[key].extend(outcomes)
            method_expanded.setdefault(key, []).extend(expanded_results[test_id])
            method_starts.setdefault(key, []).extend(starts.get(test_id, []))

    # 結果を出力
    for method_key in sorted(method_outcomes.keys()):
        outcomes = method_outcomes[method_key]
        if any(o == "Failed" for o in outcomes):
            result = "Failed"
        else:
            result = "Passed"
        timestamp = ""
        if with_start_time:
            values = method_starts.get(method_key, [])
            formatted = (subprocess.check_output(
                ["date", "-d", min(values)[1], "+%Y-%m-%d %H:%M:%S %Z"],
                text=True, encoding="utf-8").strip() if values else fallback_start_time)
            timestamp = "\t" + formatted
        if with_counts:
            # 未実行や集約された Theory の結果ではレコード数を推測しない。
            count = sum(o in ("Passed", "Failed") for o in outcomes)
            complete = all(o in ("Passed", "Failed") for o in outcomes)
            if len(outcomes) == 1 and not all(method_expanded[method_key]):
                count = "-"
            print(f"{method_key}\t{result}\t{count}\t{'Complete' if complete else 'Partial'}{timestamp}")
        else:
            print(f"{method_key}\t{result}{timestamp}")


def main():
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        pass

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trx_file")
    parser.add_argument("--with-counts", action="store_true")
    parser.add_argument("--with-start-time", action="store_true")
    parser.add_argument("--fallback-start-time", default="")
    args = parser.parse_args()
    parse_trx(args.trx_file, args.with_counts, args.with_start_time, args.fallback_start_time)


if __name__ == '__main__':
    main()
