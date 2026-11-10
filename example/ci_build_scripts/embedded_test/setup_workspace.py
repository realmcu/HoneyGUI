#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
"""Initialize or update the workspace used by HoneyGUI embedded CI."""

import argparse
import configparser
import shlex
import shutil
import subprocess
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parents[2]
DEFAULT_MANIFEST_URL = "ssh://cn4soc.rtkbf.com:29418/hmi/manifest"
DEFAULT_MANIFEST_REVISION = "master"
DEFAULT_MANIFEST_FILE = "test/rtl8773gtp-eBadge-wh.yml"


def find_enclosing_workspace(path):
    for candidate in (path, *path.parents):
        if (candidate / ".west" / "config").is_file():
            return candidate
    return None


def workspace_manifest_file(workspace):
    config = configparser.ConfigParser()
    config.read(str(workspace / ".west" / "config"))
    return config.get("manifest", "file", fallback="")


def default_workspace():
    enclosing = find_enclosing_workspace(PROJECT_ROOT)
    if enclosing is None:
        return PROJECT_ROOT.parent / "honeygui-embedded-workspace"

    if workspace_manifest_file(enclosing) == DEFAULT_MANIFEST_FILE:
        return enclosing

    return enclosing.with_name(
        "{}-honeygui-embedded".format(enclosing.name))


DEFAULT_WORKSPACE = default_workspace()


def command_text(command):
    return " ".join(shlex.quote(str(part)) for part in command)


def find_west(command):
    candidate = Path(command).expanduser()
    if candidate.is_file():
        return str(candidate.resolve())

    resolved = shutil.which(command)
    if resolved:
        return resolved

    raise RuntimeError("west is not available on PATH: {}".format(command))


def run(command, cwd, dry_run):
    print("$ {}".format(command_text(command)), flush=True)
    if not dry_run:
        subprocess.check_call(command, cwd=str(cwd))


def is_initialized(workspace):
    return (workspace / ".west" / "config").is_file()


def validate_workspace(workspace, dry_run):
    if is_initialized(workspace):
        return

    if workspace.exists() and any(workspace.iterdir()):
        raise RuntimeError(
            "Workspace is not empty and has no .west/config: {}".format(
                workspace))

    if not dry_run:
        workspace.mkdir(parents=True, exist_ok=True)


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "workspace",
        nargs="?",
        type=Path,
        default=DEFAULT_WORKSPACE,
        help="workspace directory (default: %(default)s)",
    )
    parser.add_argument(
        "--manifest-url",
        default=DEFAULT_MANIFEST_URL,
        help="west manifest repository URL",
    )
    parser.add_argument(
        "--manifest-revision",
        default=DEFAULT_MANIFEST_REVISION,
        help="west manifest repository revision",
    )
    parser.add_argument(
        "--manifest-file",
        default=DEFAULT_MANIFEST_FILE,
        help="manifest file inside the manifest repository",
    )
    parser.add_argument(
        "--west",
        default="west",
        help="west command or executable path",
    )
    parser.add_argument(
        "--skip-update",
        action="store_true",
        help="initialize the workspace without running west update",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="print commands without creating or updating the workspace",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    workspace = args.workspace.expanduser().resolve()
    west = find_west(args.west)

    print("Embedded CI workspace: {}".format(workspace))
    validate_workspace(workspace, args.dry_run)

    if is_initialized(workspace):
        print("West workspace is already initialized; skipping west init.")
    else:
        init_command = [
            west,
            "init",
            "-m",
            args.manifest_url,
            "--mr",
            args.manifest_revision,
            "--mf",
            args.manifest_file,
            ".",
        ]
        run(init_command, workspace, args.dry_run)

    if not args.skip_update:
        run([west, "update"], workspace, args.dry_run)

    print("Embedded CI workspace is ready.")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        print("[FAIL] {}".format(error), file=sys.stderr)
        sys.exit(1)
