#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
#!/usr/bin/env python3

import argparse
import codecs
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path


if sys.platform == "win32":
    sys.stdout = codecs.getwriter("utf-8")(sys.stdout.buffer, "strict")
    sys.stderr = codecs.getwriter("utf-8")(sys.stderr.buffer, "strict")


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parent.parent
SIMULATOR_DIR = PROJECT_ROOT / "win32_sim"
BUILD_ROOT = PROJECT_ROOT / ".codex-build" / "widget-tests"
CONFIG_FILES = (".config", ".config.old", "cconfig.h", "config.cmake", "menu_config.py")
EXPECTED_UNSUPPORTED_DEMOS = {
    "REALTEK_BUILD_EXAMPLE_GRAY_WIDGET",
    "REALTEK_BUILD_REAL_PAINTENGINE",
    "REALTEK_BUILD_REAL_NANOVG",
    "REALTEK_BUILD_TEST",
    "REALTEK_BUILD_HONEYGUI_AUTO_TEST",
}


def parse_args():
    parser = argparse.ArgumentParser(description="Build every HoneyGUI demo")
    parser.add_argument(
        "--build-system",
        choices=("all", "scons", "cmake"),
        default="all",
        help="build system to test (default: all)",
    )
    parser.add_argument("--jobs", type=int, default=8, help="parallel build jobs")
    return parser.parse_args()


def parse_kconfig():
    """Parse Kconfig.gui and return the demo choice entries."""
    content = (PROJECT_ROOT / "Kconfig.gui").read_text(encoding="ascii")
    choice_match = re.search(
        r'choice\s+prompt "Select HoneyGUI Demo".*?endchoice', content, re.DOTALL
    )
    if not choice_match:
        return []

    config_pattern = r'config\s+(REALTEK_BUILD_\w+)\s+bool\s+"([^"]+)"'
    return re.findall(config_pattern, choice_match.group(0))


def find_build_support(config_name):
    symbol = "CONFIG_{}".format(config_name)
    support = {}
    for build_system, filename in (("SCons", "SConscript"), ("CMake", "CMakeLists.txt")):
        support[build_system] = any(
            symbol in path.read_text(encoding="ascii", errors="ignore")
            for path in (PROJECT_ROOT / "example").rglob(filename)
        )
    return support


def write_config(config_name):
    lines = [
        "CONFIG_REALTEK_HONEYGUI=y",
        "CONFIG_{}=y".format(config_name),
        "CONFIG_REALTEK_BUILD_LETTER_SHELL=y",
        "CONFIG_REALTEK_BUILD_GUI_BOX2D=y",
        "CONFIG_REALTEK_BUILD_LITE3D=y",
        "CONFIG_REALTEK_BUILD_XML_LOADER=y",
    ]
    if config_name == "REALTEK_BUILD_PARTICLE_DEMO":
        lines.append("CONFIG_REALTEK_BUILD_PARTICLE_SYSTEM=y")
    (SIMULATOR_DIR / ".config").write_text("\n".join(lines) + "\n", encoding="ascii")


def run_command(command, cwd):
    return subprocess.run(
        command,
        cwd=str(cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding="utf-8",
        errors="ignore",
    )


def build_scons(jobs):
    subprocess.run(
        ["scons", "-c"],
        cwd=str(SIMULATOR_DIR),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    result = run_command(["scons", "-j{}".format(jobs)], SIMULATOR_DIR)
    subprocess.run(
        ["scons", "-c"],
        cwd=str(SIMULATOR_DIR),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return result


def build_cmake(config_name, jobs):
    build_dir = BUILD_ROOT / config_name.lower()
    if build_dir.exists():
        shutil.rmtree(str(build_dir))

    generator = "MinGW Makefiles" if sys.platform == "win32" else "Unix Makefiles"
    configure = run_command(
        [
            "cmake",
            "-G",
            generator,
            "-S",
            str(SIMULATOR_DIR),
            "-B",
            str(build_dir),
        ],
        PROJECT_ROOT,
    )
    if configure.returncode != 0:
        return configure

    return run_command(
        ["cmake", "--build", str(build_dir), "--parallel", str(jobs)], PROJECT_ROOT
    )


def snapshot_files():
    snapshot = {}
    for name in CONFIG_FILES:
        path = SIMULATOR_DIR / name
        snapshot[path] = path.read_bytes() if path.exists() else None
    return snapshot


def restore_files(snapshot):
    for path, content in snapshot.items():
        if content is None:
            if path.exists():
                path.unlink()
        else:
            path.write_bytes(content)


def print_summary(results):
    print("\n" + "=" * 70)
    print("SUMMARY")
    print("=" * 70)

    failed = []
    skipped = []
    for build_system, name, status, output in results:
        if status == "SUCCESS":
            symbol = "[OK]"
        elif status == "SKIPPED":
            symbol = "[SKIP]"
            skipped.append((build_system, name, output))
        else:
            symbol = "[FAIL]"
        print("{} {:<6} {}: {}".format(symbol, build_system, name, status))
        if status == "FAILED":
            failed.append((build_system, name, output))

    success_count = sum(1 for _, _, status, _ in results if status == "SUCCESS")
    print(
        "\nTotal: {} passed, {} failed, {} skipped".format(
            success_count, len(failed), len(skipped)
        )
    )

    if failed:
        log_path = SCRIPT_DIR / "build_errors.log"
        with log_path.open("w", encoding="utf-8") as log:
            for build_system, name, output in failed:
                heading = "{} - {}".format(build_system, name)
                print("\n{}:\n{}".format(heading, output[:300]))
                log.write("\n{}\n{}\n{}\n".format("=" * 60, heading, "=" * 60))
                log.write(output + "\n")

    print("\n" + "=" * 70)
    print("OVERALL RESULT")
    print("=" * 70)
    print("Result: {}".format("FAILED" if failed else "SUCCESS"))
    return 1 if failed else 0


def main():
    args = parse_args()
    widgets = parse_kconfig()
    if not widgets:
        print("No demos found in Kconfig.gui")
        return 1

    build_system_names = {"scons": "SCons", "cmake": "CMake"}
    build_systems = ("SCons", "CMake") if args.build_system == "all" else (
        build_system_names[args.build_system],
    )
    print("Found {} demos in Kconfig.gui".format(len(widgets)))
    print("Build systems: {}\n".format(", ".join(build_systems)))

    snapshot = snapshot_files()
    results = []
    try:
        for config_name, display_name in widgets:
            support = find_build_support(config_name)
            if not any(support.values()):
                output = "No SCons or CMake build description"
                if config_name in EXPECTED_UNSUPPORTED_DEMOS:
                    print("[SKIP] {}: {}".format(display_name, output))
                    results.append(("Both", display_name, "SKIPPED", output))
                else:
                    print("[FAIL] {}: {}".format(display_name, output))
                    results.append(("Both", display_name, "FAILED", output))
                continue

            write_config(config_name)
            for build_system in build_systems:
                print("=" * 70)
                print("Testing: {} [{}]".format(display_name, build_system))
                print("Config: {}".format(config_name))
                print("=" * 70)

                if not support[build_system]:
                    output = "Missing {} build description for CONFIG_{}".format(
                        build_system, config_name
                    )
                    status = "FAILED"
                else:
                    try:
                        if build_system == "SCons":
                            result = build_scons(args.jobs)
                        else:
                            result = build_cmake(config_name, args.jobs)
                        output = result.stdout or ""
                        status = "SUCCESS" if result.returncode == 0 else "FAILED"
                    except OSError as error:
                        output = str(error)
                        status = "FAILED"

                print("[{}] Build {}: {}\n".format(status, status, display_name))
                if status == "FAILED":
                    print("Error: {}\n".format(output[:500]))
                results.append((build_system, display_name, status, output))
    finally:
        try:
            if "SCons" in build_systems:
                subprocess.run(
                    ["scons", "-c"],
                    cwd=str(SIMULATOR_DIR),
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL,
                )
        except OSError:
            pass
        finally:
            restore_files(snapshot)

    return print_summary(results)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\nUser interrupted test")
        print("Result: INTERRUPTED")
        sys.exit(1)
