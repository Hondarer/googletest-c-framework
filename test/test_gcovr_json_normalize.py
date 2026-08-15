#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gcovr JSON の正規化と分岐統合を検証する。"""

import importlib.util
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT_PATH = Path(__file__).parents[1] / "bin" / "gcovr_json_normalize.py"
SPEC = importlib.util.spec_from_file_location("gcovr_json_normalize", SCRIPT_PATH)
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


def detect_json_format_version():
    """実行環境の gcovr が受け付ける JSON トレースファイルの版を取得する。

    gcovr は自身の版と異なる `gcovr/format_version` のトレースファイルを
    読み取り時に拒否するため、期待値を固定するとテストが gcovr の版に依存する。
    """
    with tempfile.TemporaryDirectory() as temp_dir_text:
        temp_dir = Path(temp_dir_text)
        output_path = temp_dir / "version.json"
        subprocess.run(
            ["gcovr", "--root", str(temp_dir), "--json", "--output", str(output_path)],
            check=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        data = json.loads(output_path.read_text(encoding="utf-8"))

    return data["gcovr/format_version"]


class GcovrJsonNormalizeTest(unittest.TestCase):
    def test_normalize_keeps_only_test_sources(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            target = workspace / "app/sample/prod/libsrc/sample.c"
            source_map = MODULE.build_source_map(workspace, [str(target)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {"file": "sample.c", "lines": []},
                    {"file": "sample.inject.c", "lines": []},
                    {"file": "dependency.c", "lines": []},
                ],
            }

            normalized = MODULE.normalize_coverage(data, source_map)

        self.assertEqual(
            ["app/sample/prod/libsrc/sample.c"],
            [item["file"] for item in normalized["files"]],
        )

    def test_warns_when_no_test_source_matches(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            input_path = workspace / "raw.json"
            output_path = workspace / "normalized.json"
            data = {
                "gcovr/format_version": "0.11",
                "files": [{"file": "other.c", "lines": []}],
            }
            input_path.write_text(json.dumps(data), encoding="utf-8")

            completed = subprocess.run(
                [
                    sys.executable,
                    str(SCRIPT_PATH),
                    str(input_path),
                    str(output_path),
                    str(workspace),
                    str(workspace / "app/sample/prod/libsrc/sample.c"),
                ],
                capture_output=True,
                text=True,
                check=True,
            )
            normalized = json.loads(output_path.read_text(encoding="utf-8"))

        self.assertIn("no TEST_SRCS matched", completed.stderr)
        self.assertEqual([], normalized["files"])

    def test_keeps_all_sources_when_multiple_are_given(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            sources = [
                str(workspace / "app/sample/prod/libsrc/sample_linux.c"),
                str(workspace / "app/sample/prod/libsrc/sample_windows.c"),
            ]
            source_map = MODULE.build_source_map(workspace, sources)
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {"file": "sample_linux.c", "lines": []},
                    {"file": "sample_windows.c", "lines": []},
                ],
            }

            normalized = MODULE.normalize_coverage(data, source_map)

        self.assertEqual(
            [
                "app/sample/prod/libsrc/sample_linux.c",
                "app/sample/prod/libsrc/sample_windows.c",
            ],
            [item["file"] for item in normalized["files"]],
        )

    def test_source_map_rejects_duplicate_basenames(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            sources = [
                str(workspace / "first/sample.c"),
                str(workspace / "second/sample.c"),
            ]

            with self.assertRaisesRegex(ValueError, "duplicate basenames"):
                MODULE.build_source_map(workspace, sources)

    def test_eh_arc_marker_drops_zero_count_on_call_line(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "name.push_back(ch); /* TESTFW_EXCL_EH_ARCS */\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": [
                                    {"count": 3, "fallthrough": True, "throw": False},
                                    {"count": 0, "fallthrough": False, "throw": False},
                                ],
                            }
                        ],
                    }
                ],
            }

            MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

        self.assertEqual(1, len(data["files"][0]["lines"][0]["branches"]))
        self.assertEqual(3, data["files"][0]["lines"][0]["branches"][0]["count"])

    def test_eh_arc_marker_on_continuation_applies_to_statement_start(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "found = search(\n"
                "    units, /* TESTFW_EXCL_EH_ARCS */\n"
                "    flag);\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": [
                                    {"count": 2, "fallthrough": True, "throw": False},
                                    {"count": 0, "fallthrough": False, "throw": False},
                                ],
                            }
                        ],
                    }
                ],
            }

            MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

        self.assertEqual(1, len(data["files"][0]["lines"][0]["branches"]))

    def test_eh_arc_marker_does_not_slide_onto_decision_line(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "const bool decoded = utf8_decode(text); /* TESTFW_EXCL_EH_ARCS */\n"
                "if (!decoded)\n"
                "{\n"
                "    return false;\n"
                "}\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": [
                                    {"count": 1, "fallthrough": True, "throw": False},
                                ],
                            },
                            {
                                "line_number": 2,
                                "count": 1,
                                "branches": [
                                    {"count": 1, "fallthrough": True, "throw": False},
                                    {"count": 0, "fallthrough": False, "throw": False},
                                ],
                            },
                        ],
                    }
                ],
            }

            MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

        if_branches = data["files"][0]["lines"][1]["branches"]
        self.assertEqual(2, len(if_branches))
        self.assertEqual(0, if_branches[1]["count"])

    def test_eh_arc_contract_drops_matching_uncovered_branches(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "if (!decode()) /* TESTFW_EXCL_EH_ARCS: 1/4 */\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": [
                                    {"count": 10, "fallthrough": True, "throw": False},
                                    {"count": 2, "fallthrough": True, "throw": False},
                                    {"count": 8, "fallthrough": False, "throw": False},
                                    {"count": 0, "fallthrough": False, "throw": False},
                                ],
                            }
                        ],
                    }
                ],
            }

            MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

        branches = data["files"][0]["lines"][0]["branches"]
        self.assertEqual(3, len(branches))
        self.assertTrue(all(branch["count"] > 0 for branch in branches))

    def test_eh_arc_contract_mismatch_raises(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "if (!decode()) /* TESTFW_EXCL_EH_ARCS: 1/4 */\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": [
                                    {"count": 10, "fallthrough": True, "throw": False},
                                    {"count": 0, "fallthrough": False, "throw": False},
                                ],
                            }
                        ],
                    }
                ],
            }

            with self.assertRaisesRegex(ValueError, "expected 1/4, found 1/2"):
                MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

    def test_eh_arc_decision_line_requires_ratio(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "if (!decode()) /* TESTFW_EXCL_EH_ARCS */\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": [
                                    {"count": 1, "fallthrough": True, "throw": False},
                                    {"count": 0, "fallthrough": False, "throw": False},
                                ],
                            }
                        ],
                    }
                ],
            }

            with self.assertRaisesRegex(ValueError, "decision line requires"):
                MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

    def test_eh_arc_already_covered_is_noop(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            workspace = Path(temp_dir_text)
            source = workspace / "app/sample/prod/libsrc/sample.cc"
            source.parent.mkdir(parents=True)
            source.write_text(
                "name.push_back(ch); /* TESTFW_EXCL_EH_ARCS: 1/2 */\n",
                encoding="utf-8",
            )
            source_map = MODULE.build_source_map(workspace, [str(source)])
            branches = [
                {"count": 4, "fallthrough": True, "throw": False},
            ]
            data = {
                "gcovr/format_version": "0.11",
                "files": [
                    {
                        "file": "app/sample/prod/libsrc/sample.cc",
                        "lines": [
                            {
                                "line_number": 1,
                                "count": 1,
                                "branches": list(branches),
                            }
                        ],
                    }
                ],
            }

            MODULE.apply_eh_arc_exclusions(data, workspace, source_map)

        self.assertEqual(1, len(data["files"][0]["lines"][0]["branches"]))

    @unittest.skipUnless(
        sys.platform == "linux",
        "gcovr の分岐統合テストは Linux のカバレッジ環境で実行します。",
    )
    def test_gcovr_merge_preserves_branch_identity(self):
        with tempfile.TemporaryDirectory() as temp_dir_text:
            temp_dir = Path(temp_dir_text)
            first_path = temp_dir / "first.json"
            duplicate_path = temp_dir / "duplicate.json"
            complement_path = temp_dir / "complement.json"
            duplicate_output = temp_dir / "duplicate-output.json"
            complement_output = temp_dir / "complement-output.json"

            self.write_tracefile(first_path, 1, 0)
            self.write_tracefile(duplicate_path, 1, 0)
            self.write_tracefile(complement_path, 0, 1)

            self.merge_tracefiles(first_path, duplicate_path, duplicate_output)
            self.merge_tracefiles(first_path, complement_path, complement_output)

            self.assertEqual((1, 2), self.branch_coverage(duplicate_output))
            self.assertEqual((2, 2), self.branch_coverage(complement_output))

    @staticmethod
    def write_tracefile(path, fallthrough_count, taken_count):
        data = {
            "gcovr/format_version": detect_json_format_version(),
            "files": [
                {
                    "file": "app/sample/prod/libsrc/sample.c",
                    "lines": [
                        {
                            "line_number": 1,
                            "count": 1,
                            "branches": [
                                # gcovr は分岐を始点と終点のブロック ID で識別する。
                                # see: https://gcovr.com/en/8.6/output/json.html#branch-entries
                                {
                                    "source_block_id": 0,
                                    "destination_block_id": 1,
                                    "count": fallthrough_count,
                                    "fallthrough": True,
                                    "throw": False,
                                },
                                {
                                    "source_block_id": 0,
                                    "destination_block_id": 2,
                                    "count": taken_count,
                                    "fallthrough": False,
                                    "throw": False,
                                },
                            ],
                            "gcovr/md5": "0123456789abcdef0123456789abcdef",
                        }
                    ],
                    "functions": [],
                }
            ],
        }
        path.write_text(json.dumps(data), encoding="utf-8")

    @staticmethod
    def merge_tracefiles(first_path, second_path, output_path):
        subprocess.run(
            [
                "gcovr",
                "--add-tracefile",
                str(first_path),
                "--add-tracefile",
                str(second_path),
                "--json",
                "--output",
                str(output_path),
            ],
            check=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )

    @staticmethod
    def branch_coverage(path):
        data = json.loads(path.read_text(encoding="utf-8"))
        branches = data["files"][0]["lines"][0]["branches"]
        covered = sum(1 for branch in branches if branch["count"] > 0)
        return covered, len(branches)


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
    unittest.main()
