#!/usr/bin/env python3
"""
TRX (Visual Studio Test Results) XML パーサー

dotnet test --logger "trx" が出力する TRX ファイルを解析し、
テストごとの結果を TSV で標準出力に出力する。

使用方法:
    parse_trx_results.py <trx_file>

Output:
    ClassName.MethodName<TAB>Passed|Failed (1行1テスト)

パラメータ付きテスト (Theory) は同一メソッド名でグループ化し、
1件でも Failed があれば Failed とする。
"""

import sys
import xml.etree.ElementTree as ET

TRX_NS = "http://microsoft.com/schemas/VisualStudio/TeamTest/2010"


def ns(tag):
    return f"{{{TRX_NS}}}{tag}"


def parse_trx(trx_path):
    tree = ET.parse(trx_path)
    root = tree.getroot()

    # testId -> (className, methodName) のマッピングを構築
    test_id_map = {}
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

    # testId -> outcome のマッピングを構築
    results_map = {}
    results_elem = root.find(ns("Results"))
    if results_elem is not None:
        for result in results_elem.findall(ns("UnitTestResult")):
            test_id = result.get("testId")
            outcome = result.get("outcome", "NotExecuted")
            if test_id in results_map:
                results_map[test_id].append(outcome)
            else:
                results_map[test_id] = [outcome]

    # メソッド単位でグループ化 (パラメータ付きテスト対応)
    # key: "ClassName.MethodName", value: list of outcomes
    method_outcomes = {}
    for test_id, outcomes in results_map.items():
        if test_id in test_id_map:
            short_class, method_name = test_id_map[test_id]
            # パラメータ付きテストのメソッド名からパラメータ部分を除去
            # 例: "Add_ShouldReturnCorrectResult(a: 10, b: 20, expected: 30)"
            #  -> "Add_ShouldReturnCorrectResult"
            base_method = method_name.split("(")[0]
            key = f"{short_class}.{base_method}"
            if key not in method_outcomes:
                method_outcomes[key] = []
            method_outcomes[key].extend(outcomes)

    # 結果を出力
    for method_key in sorted(method_outcomes.keys()):
        outcomes = method_outcomes[method_key]
        if any(o == "Failed" for o in outcomes):
            result = "Failed"
        else:
            result = "Passed"
        print(f"{method_key}\t{result}")


def main():
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except AttributeError:
        pass

    if len(sys.argv) != 2:
        print("Usage: parse_trx_results.py <trx_file>", file=sys.stderr)
        sys.exit(1)

    parse_trx(sys.argv[1])


if __name__ == '__main__':
    main()
