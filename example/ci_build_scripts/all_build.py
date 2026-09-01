#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
"""Run every HoneyGUI CI build stage and write a combined report.

Two contracts must not change:
  * script/ci-build.py greps stdout for the exact string
    "FINAL RESULT: [OK] ALL TESTS PASSED"
  * .github/workflows/build-test.yml uploads test_report.txt
"""

import argparse
import datetime
import os
import subprocess
import sys

import ci_common

PASS_MARKER = "FINAL RESULT: [OK] ALL TESTS PASSED"
FAIL_MARKER = "FINAL RESULT: [FAIL] SOME TESTS FAILED"
REPORT_NAME = "test_report.txt"

# (stage name, script, extra args)
STAGES = [
    ("Library build (CMake)", "lib_build_by_cmake.py", []),
    ("Widget demos (SCons)", "widget_build_by_scons.py", []),
    ("Widget demos (CMake)", "widget_build_by_cmake.py", []),
    ("Application demos (SCons)", "application_build_by_scons.py", []),
    ("Application demos (CMake)", "application_build_by_cmake.py", []),
]

def run_stage(name, script, extra, jobs):
    path = ci_common.SCRIPT_DIR / script
    print("\n" + "=" * 70)
    print("Running: {}".format(name))
    print("Script:  {} {}".format(script, " ".join(extra)))
    print("=" * 70 + "\n", flush=True)

    if not path.exists():
        print("[ERROR] Script not found: {}\n".format(path))
        return {"status": "NOT_FOUND", "return_code": -1,
                "output": "Script not found: {}".format(path)}

    cmd = [sys.executable, str(path), "--jobs", str(jobs)] + extra
    lines = []
    process = subprocess.Popen(
        cmd, cwd=str(ci_common.SCRIPT_DIR),
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, encoding="utf-8", errors="ignore", bufsize=1,
    )
    try:
        for line in process.stdout:
            print(line, end="")
            lines.append(line)
        code = process.wait()
    except KeyboardInterrupt:
        process.kill()
        raise
    return {"status": "SUCCESS" if code == 0 else "FAILED",
            "return_code": code, "output": "".join(lines)}


def write_report(results, start, end):
    path = ci_common.SCRIPT_DIR / REPORT_NAME
    with path.open("w", encoding="utf-8") as f:
        f.write("=" * 70 + "\n")
        f.write("HoneyGUI - CI Build Report\n")
        f.write("=" * 70 + "\n\n")
        f.write("Start Time: {}\n".format(start.strftime("%Y-%m-%d %H:%M:%S")))
        f.write("End Time:   {}\n".format(end.strftime("%Y-%m-%d %H:%M:%S")))
        f.write("Duration:   {}\n\n".format(end - start))
        f.write("=" * 70 + "\nDETAILED RESULTS\n" + "=" * 70 + "\n")
        for name, result in results.items():
            f.write("\nStage:       {}\n".format(name))
            f.write("Status:      {}\n".format(result["status"]))
            f.write("Return Code: {}\n".format(result["return_code"]))
            f.write("-" * 70 + "\nOutput:\n")
            f.write(result.get("output", "No output") + "\n")
            f.write("=" * 70 + "\n")
    print("\nReport saved to: {}".format(path))


def preflight(stages):
    """Check only the tools required by the selected stages."""
    stage_text = [
        "{} {}".format(script, " ".join(extra))
        for _, script, extra in stages
    ]
    needs_scons = any("scons" in text for text in stage_text)
    needs_cmake = any("cmake" in text for text in stage_text)
    errors = ci_common.validate_environment(
        needs_scons=needs_scons,
        needs_cmake=needs_cmake,
    )
    if any(not text.startswith("lib_build_by_cmake.py")
           for text in stage_text):
        errors += ci_common.validate_static_configs()
    return errors


def main():
    ci_common.fix_console_encoding()
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 8,
                        help="parallel build jobs passed to each stage")
    parser.add_argument("--skip", nargs="+", default=[], metavar="SCRIPT",
                        help="stage scripts to skip, e.g. lib_build_by_cmake.py")
    args = parser.parse_args()

    stages = list(STAGES)
    stages = [s for s in stages if s[1] not in args.skip]

    start = datetime.datetime.now()
    print("\n" + "=" * 70)
    print("HoneyGUI - CI Build Suite")
    print("=" * 70)
    print("Start Time: {}".format(start.strftime("%Y-%m-%d %H:%M:%S")))
    print("Stages:     {}".format(len(stages)))

    results = {}
    errors = preflight(stages)
    if errors:
        output = "\n".join(errors)
        print("\n[FAIL] CI preflight:\n{}".format(output))
        results["CI preflight"] = {
            "status": "FAILED",
            "return_code": 1,
            "output": output,
        }

    for name, script, extra in stages:
        if errors:
            break
        results[name] = run_stage(name, script, extra, args.jobs)
    end = datetime.datetime.now()

    print("\n" + "=" * 70)
    print("BUILD SUITE SUMMARY")
    print("=" * 70 + "\n")
    print("{:<44} {:<10} {}".format("Stage", "Status", "Return Code"))
    print("-" * 70)
    failed = 0
    for name, result in results.items():
        status = result["status"]
        symbol = "[OK]" if status == "SUCCESS" else "[FAIL]"
        if status != "SUCCESS":
            failed += 1
        print("{:<44} {} {:<8} {}".format(
            name, symbol, status, result["return_code"]))
    print("-" * 70)
    print("\nTotal Stages: {}".format(len(results)))
    print("Passed: {}".format(len(results) - failed))
    print("Failed: {}".format(failed))
    print("\nStart Time: {}".format(start.strftime("%Y-%m-%d %H:%M:%S")))
    print("End Time:   {}".format(end.strftime("%Y-%m-%d %H:%M:%S")))
    print("Duration:   {}".format(end - start))

    write_report(results, start, end)

    print("\n" + "=" * 70)
    if results and failed == 0:
        print(PASS_MARKER)
        print("=" * 70)
        return 0
    print(FAIL_MARKER)
    print("=" * 70)
    return 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\n\nUser interrupted build suite")
        print(FAIL_MARKER)
        sys.exit(1)
