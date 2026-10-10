#!/usr/bin/env python3
"""Google Test の展開済み一覧から、テスト定義ごとのエビデンスを生成する。"""

import argparse
import json
from pathlib import Path
import subprocess
import sys

from test_summary import SummaryError, text_encoding
from test_subprocedures import SourceIndex, discover


HERE = Path(__file__).resolve().parent


def test_names(text):
    # exec_test_c_cpp.sh の list_tests が suite を補った行を受け取る。
    return list(dict.fromkeys(line.split()[0] for line in text.splitlines() if line.strip()))


def definition_id(name):
    suite, test = name.split(".", 1)
    # 値パラメーター テストだけを統合する。型パラメーターの suite/0 は維持する。
    if "/" in test:
        return suite.rsplit("/", 1)[-1] + "." + test.split("/", 1)[0]
    return name


def record_path(name):
    parts = name.split("/")
    if len(parts) == 3:
        return f"{parts[1]}/{parts[0]}/{parts[2]}"
    return name


def group_tests(full, selected):
    groups = {}
    for name in full:
        key = definition_id(name)
        groups.setdefault(key, {"all": [], "selected": []})["all"].append(name)
    for name in selected:
        key = definition_id(name)
        if key not in groups or name not in groups[key]["all"]:
            raise SummaryError(f"展開済みの全テスト一覧に存在しません: {name}")
        groups[key]["selected"].append(name)
    return {key: value for key, value in groups.items() if value["selected"] and "/" in value["selected"][0].split(".", 1)[1]}


def extract_code(name, source, is_windows, encoding):
    result = subprocess.run(
        ["awk", "-v", f"test_id={name}", "-v", f"is_windows={is_windows}",
         "-f", str(HERE / "get_test_code_c_cpp.awk")],
        input=source, text=True, encoding=encoding, capture_output=True,
    )
    if result.returncode or not result.stdout.strip():
        raise SummaryError(f"{name}: テストコードを抽出できません: {result.stderr.strip()}")
    return result.stdout


def prepare(full, selected, manifest, is_windows, encoding):
    groups = group_tests(test_names(full), test_names(selected))
    index = SourceIndex.from_paths(discover("c_cpp"), is_windows=is_windows == "1", encoding=encoding) if groups else None
    for key, group in groups.items():
        partial = set(group["all"]) != set(group["selected"])
        prefixes = {name.split("/", 1)[0] for name in group["all"]}
        counts = {}
        group["evidence"] = index.report(key, True, len(group["all"]), partial, prefixes=prefixes, counts=counts)
        group["counts"] = None if partial else counts
    # 全定義の解析が成功してから、一時記録だけを作る。
    Path(manifest).write_text(json.dumps(groups, ensure_ascii=False), encoding="utf-8")


def finish(manifest, encoding, counts_dir=None):
    from results_markdown import definition, execution_output, write
    groups = json.loads(Path(manifest).read_text(encoding="utf-8"))
    for key, group in groups.items():
        records = []
        for name in group["selected"]:
            record = Path("results") / record_path(name) / "results.md"
            content = record.read_text(encoding="utf-8")
            status = next(line.split(": ", 1)[1] for line in content.splitlines() if line.startswith("- 判定: "))
            output = execution_output(content)
            records.append({"name": name, "status": status,
                            "path": record_path(name)[len(key) + 1:] + "/results.md", "output": output})
        write(Path("results") / key / "results.md", definition(key, group["evidence"], records, len(group["all"])))
        if counts_dir:
            path = Path(counts_dir) / (key + ".json")
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(json.dumps(group["counts"], ensure_ascii=False), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=["prepare", "finish"])
    parser.add_argument("manifest")
    parser.add_argument("--full-list")
    parser.add_argument("--selected-list")
    parser.add_argument("--is-windows", choices=["0", "1"], default="0")
    parser.add_argument("--encoding", default="utf-8")
    parser.add_argument("--counts-dir")
    args = parser.parse_args()
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
        sys.stderr.reconfigure(encoding="utf-8")
    try:
        encoding = text_encoding(args.encoding)
        if args.mode == "prepare":
            prepare(Path(args.full_list).read_text(encoding=encoding, errors="replace"),
                    Path(args.selected_list).read_text(encoding=encoding, errors="replace"),
                    args.manifest, args.is_windows, encoding)
        else:
            finish(args.manifest, encoding, args.counts_dir)
    except (SummaryError, OSError, ValueError, LookupError) as error:
        print(f"[  FAILED  ] {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
