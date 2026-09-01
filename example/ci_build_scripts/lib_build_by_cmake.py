#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
"""Build the prebuilt GUI libraries under lib/ with CMake.

Each standalone lib/<tool>/ CMake project has its own .config. Toolchains that
ship per-SoC settings expose them as RTL*.cmake and are built once per SoC via
-DSOC=<name>; lib/gcc has none and is built once with the host compiler.

Missing toolchains are reported as FAILED, same as before -- silently skipping
them would hide a loss of CI coverage. Use --tools to narrow the run instead,
e.g. `--tools gcc` on a developer machine without the ARM compilers.
"""

import argparse
import os
import shutil
import sys
from pathlib import Path

import ci_common


def discover_targets(tools=None):
    """Yield (tool, soc_or_None) for every buildable lib target."""
    targets = []
    for tool_dir in sorted(p for p in ci_common.LIB_DIR.iterdir() if p.is_dir()):
        if not (tool_dir / "CMakeLists.txt").exists():
            continue
        if not (tool_dir / ".config").exists():
            continue
        tool = tool_dir.name
        if tools and tool not in tools:
            continue
        socs = sorted(p.stem for p in tool_dir.glob("RTL*.cmake"))
        if socs:
            targets += [(tool, soc) for soc in socs]
        else:
            targets.append((tool, None))
    return targets


def build_one(tool, soc, jobs):
    """Configure and build one lib target; returns (returncode, output)."""
    source_dir = ci_common.LIB_DIR / tool
    name = tool if soc is None else "{}-{}".format(tool, soc)
    build_dir = ci_common.make_build_dir(name, "lib")

    configure = ["cmake", "-G", ci_common.CMAKE_GENERATOR,
                 "-S", str(source_dir), "-B", str(build_dir),
                 "-DHONEYGUI_PYTHON={}".format(
                     Path(sys.executable).as_posix())]
    if soc is not None:
        configure.append("-DSOC={}".format(soc))

    try:
        result = ci_common.run_command(configure, ci_common.PROJECT_ROOT)
        if result.returncode != 0:
            return (result.returncode,
                    "CMake configure failed:\n" + (result.stdout or ""))

        result = ci_common.run_command(
            ["cmake", "--build", str(build_dir), "--parallel", str(jobs)],
            ci_common.PROJECT_ROOT,
        )
        return result.returncode, result.stdout or ""
    finally:
        shutil.rmtree(str(build_dir), ignore_errors=True)


def snapshot_generated():
    """lib builds rewrite gui_version.h and each lib/<tool>/config.cmake.

    Both are tracked files regenerated from the current commit / .config, so a
    build sweep would otherwise leave the working tree dirty.
    """
    paths = [ci_common.PROJECT_ROOT / "realgui" / "gui_version.h"]
    paths += sorted(ci_common.LIB_DIR.glob("*/config.cmake"))
    return {p: (p.read_bytes() if p.exists() else None) for p in paths}


def restore_generated(snapshot):
    for path, content in snapshot.items():
        if content is None:
            if path.exists():
                path.unlink()
        elif path.read_bytes() != content:
            path.write_bytes(content)


def main():
    ci_common.fix_console_encoding()
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 8,
                        help="parallel build jobs")
    parser.add_argument("--tools", nargs="+", metavar="TOOL",
                        help="restrict to these lib/<tool> directories")
    args = parser.parse_args()

    targets = discover_targets(args.tools)
    if not targets:
        print("No lib targets found under {}".format(ci_common.LIB_DIR))
        return 1

    report = ci_common.Report("lib targets via CMake")
    print("Found {} lib target(s)\n".format(len(targets)))

    snapshot = snapshot_generated()
    try:
        for tool, soc in targets:
            name = tool if soc is None else "{} - {}".format(tool, soc)
            print("=" * 70)
            print("Building lib: {}".format(name))
            print("=" * 70)
            code, output = build_one(tool, soc, args.jobs)
            if code == 0:
                report.ok(tool, name)
                print("[OK] {}\n".format(name))
            else:
                report.fail(tool, name, output)
                print("[FAIL] {}\n{}\n".format(name, output[-1500:]))
    finally:
        restore_generated(snapshot)
        shutil.rmtree(str(ci_common.BUILD_ROOT / "lib"), ignore_errors=True)

    return report.finish("build_errors_lib.log")


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\nInterrupted")
        sys.exit(1)
