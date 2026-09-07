#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
"""Build, flash, and capture logs for the HoneyGUI embedded CI target."""

import argparse
import os
import shutil
import subprocess
import sys
import threading
import time
from datetime import datetime
from pathlib import Path

from setup_workspace import DEFAULT_WORKSPACE
from setup_workspace import command_text
from setup_workspace import find_west
from setup_workspace import is_initialized


SCRIPT_DIR = Path(__file__).resolve().parent
SETUP_SCRIPT = SCRIPT_DIR / "setup_workspace.py"
DEFAULT_HONEYGUI_SOURCE = SCRIPT_DIR.parents[2]
DEFAULT_BOARD = "rtl87x3g_evb"
DEFAULT_FLASH_PORT = Path("/dev/ttyUSB0")
DEFAULT_LOG_PORT = Path("/dev/ttyUSB1")
DEFAULT_BAUD = 2000000
DEFAULT_LOG_SECONDS = 60.0


def run(command, cwd, dry_run, env=None):
    print("$ {}".format(command_text(command)), flush=True)
    if not dry_run:
        subprocess.check_call(command, cwd=str(cwd), env=env)


def resolve_path(base, path):
    path = path.expanduser()
    if path.is_absolute():
        return path.resolve()
    return (base / path).resolve()


def validate_workspace(workspace, application, honeygui_source):
    if not is_initialized(workspace):
        raise RuntimeError(
            "West workspace is not initialized: {}".format(workspace))
    if not (application / "CMakeLists.txt").is_file():
        raise RuntimeError(
            "Application directory is invalid: {}".format(application))
    if not (honeygui_source / "zephyr" / "module.yml").is_file():
        raise RuntimeError(
            "HoneyGUI source directory is invalid: {}".format(
                honeygui_source))


def write_console(data):
    output = getattr(sys.stdout, "buffer", None)
    if output is not None:
        output.write(data)
    else:
        sys.stdout.write(data.decode("utf-8", errors="replace"))
    sys.stdout.flush()


class SerialLogCapture:
    def __init__(self, port, baud, output):
        self.port = port
        self.baud = baud
        self.output = output
        self.process = None
        self.thread = None
        self.log = None
        self.error = None

    def start(self):
        stty = shutil.which("stty")
        stdbuf = shutil.which("stdbuf")
        cat = shutil.which("cat")
        if not stty or not stdbuf or not cat:
            raise RuntimeError("stty, stdbuf, and cat are required")
        if not self.port.exists():
            raise RuntimeError("Log port does not exist: {}".format(self.port))
        if not os.access(str(self.port), os.R_OK | os.W_OK):
            raise RuntimeError(
                "Log port is not readable and writable: {}".format(
                    self.port))

        self.output.parent.mkdir(parents=True, exist_ok=True)
        subprocess.check_call(
            [
                stty,
                "-F",
                str(self.port),
                str(self.baud),
                "cs8",
                "-cstopb",
                "-parenb",
                "-crtscts",
                "-hupcl",
                "raw",
                "-echo",
            ]
        )
        self.log = self.output.open("wb")
        self.process = subprocess.Popen(
            [stdbuf, "-o0", cat, str(self.port)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            bufsize=0,
        )
        self.thread = threading.Thread(target=self._copy_output, daemon=True)
        self.thread.start()
        print(
            "Capturing serial log from {} to {}".format(
                self.port, self.output),
            flush=True,
        )

    def _copy_output(self):
        try:
            while True:
                data = self.process.stdout.read(4096)
                if not data:
                    break
                self.log.write(data)
                self.log.flush()
                write_console(data)
        except (OSError, ValueError) as error:
            self.error = error

    def wait(self, seconds):
        if seconds == 0:
            print("Capturing log until interrupted.", flush=True)
            while self.process.poll() is None:
                time.sleep(0.2)
            return

        print(
            "Capturing log for {:.1f} seconds.".format(seconds),
            flush=True,
        )
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            if self.process.poll() is not None:
                raise RuntimeError("Serial log reader exited unexpectedly")
            remaining = max(0.0, deadline - time.monotonic())
            time.sleep(min(0.2, remaining))

    def stop(self):
        if self.process is not None and self.process.poll() is None:
            self.process.terminate()
            try:
                self.process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.process.kill()
                self.process.wait()
        if self.thread is not None:
            self.thread.join(timeout=2)
        if self.log is not None:
            self.log.close()
        if self.error is not None:
            raise RuntimeError(
                "Failed while reading serial log: {}".format(self.error))


def check_log(log_file, expected, rejected):
    if not log_file.is_file() or log_file.stat().st_size == 0:
        raise RuntimeError("Serial log is empty: {}".format(log_file))

    text = log_file.read_bytes().decode("utf-8", errors="replace")
    missing = [value for value in expected if value not in text]
    found = [value for value in rejected if value in text]
    if missing:
        raise RuntimeError(
            "Expected log text was not found: {}".format(", ".join(missing)))
    if found:
        raise RuntimeError(
            "Rejected log text was found: {}".format(", ".join(found)))


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "workspace",
        nargs="?",
        type=Path,
        default=DEFAULT_WORKSPACE,
        help="embedded west workspace (default: %(default)s)",
    )
    parser.add_argument(
        "--application",
        type=Path,
        default=Path("applications"),
        help="application path relative to the workspace",
    )
    parser.add_argument(
        "--honeygui-source",
        type=Path,
        default=DEFAULT_HONEYGUI_SOURCE,
        help="HoneyGUI source used by the embedded build",
    )
    parser.add_argument(
        "--board",
        default=DEFAULT_BOARD,
        help="west build board (default: %(default)s)",
    )
    parser.add_argument(
        "--flash-port",
        type=Path,
        default=DEFAULT_FLASH_PORT,
        help="runtime and application download port (default: %(default)s)",
    )
    parser.add_argument(
        "--log-port",
        type=Path,
        default=DEFAULT_LOG_PORT,
        help="firmware log port (default: %(default)s)",
    )
    parser.add_argument(
        "--baud",
        type=int,
        default=DEFAULT_BAUD,
        help="flash and log baud rate (default: %(default)s)",
    )
    parser.add_argument(
        "--log-seconds",
        type=float,
        default=DEFAULT_LOG_SECONDS,
        help="log capture duration; 0 waits for Ctrl+C (default: %(default)s)",
    )
    parser.add_argument(
        "--log-file",
        type=Path,
        help="log output path (default: embedded_test/log/<timestamp>.log)",
    )
    parser.add_argument(
        "--expect",
        action="append",
        default=[],
        help="text that must occur in the captured log; may be repeated",
    )
    parser.add_argument(
        "--reject",
        action="append",
        default=[],
        help="text that must not occur in the captured log; may be repeated",
    )
    parser.add_argument(
        "--west",
        default="west",
        help="west command or executable path",
    )
    parser.add_argument(
        "--pristine",
        action="store_true",
        help="run west build with a pristine build directory",
    )
    parser.add_argument(
        "--skip-setup",
        action="store_true",
        help="do not initialize or update the west workspace",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="do not build the application",
    )
    parser.add_argument(
        "--skip-runtime-flash",
        action="store_true",
        help="do not flash the runtime images",
    )
    parser.add_argument(
        "--skip-app-flash",
        action="store_true",
        help="do not flash bin/app.bin",
    )
    parser.add_argument(
        "--skip-log",
        action="store_true",
        help="do not capture the firmware serial log",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="print the complete test flow without running it",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    if args.log_seconds < 0:
        raise RuntimeError("--log-seconds cannot be negative")
    if args.skip_log and (args.expect or args.reject):
        raise RuntimeError("--expect and --reject require log capture")

    workspace = args.workspace.expanduser().resolve()
    application = resolve_path(workspace, args.application)
    honeygui_source = args.honeygui_source.expanduser().resolve()
    flash_port = args.flash_port.expanduser().resolve()
    log_port = args.log_port.expanduser().resolve()
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_file = args.log_file
    if log_file is None:
        port_name = str(log_port).replace("/", "_").lstrip("_")
        log_file = SCRIPT_DIR / "log" / "{}_{}.log".format(
            timestamp, port_name)
    else:
        log_file = log_file.expanduser().resolve()

    print("Embedded CI workspace: {}".format(workspace), flush=True)

    if not args.skip_setup:
        setup_command = [
            sys.executable,
            str(SETUP_SCRIPT),
            str(workspace),
            "--west",
            args.west,
        ]
        if args.dry_run:
            setup_command.append("--dry-run")
        run(setup_command, SCRIPT_DIR, False)

    west = find_west(args.west)
    if not args.dry_run:
        validate_workspace(workspace, application, honeygui_source)

    if not args.skip_build:
        build_command = [west, "build", "-b", args.board]
        if args.pristine:
            build_command.append("-p")
        build_command.extend(
            [
                "--",
                "-DEXTRA_ZEPHYR_MODULES={}".format(honeygui_source),
            ]
        )
        run(build_command, application, args.dry_run)
        artifact = application / "bin" / "app.bin"
        if not args.dry_run and not artifact.is_file():
            raise RuntimeError(
                "Application image was not generated: {}".format(artifact))

    runtime_script = application / "scripts" / "flash-runtime-linux.sh"
    app_script = application / "scripts" / "flash-linux-app.sh"
    flash_requested = (
        not args.skip_runtime_flash or not args.skip_app_flash)
    capture_before_flash = (
        not args.skip_log
        and flash_requested
        and log_port != flash_port
    )
    capture = None

    if args.dry_run and not args.skip_log:
        print(
            "$ stty -F {} {} cs8 -cstopb -parenb -crtscts "
            "-hupcl raw -echo".format(log_port, args.baud),
            flush=True,
        )
        print(
            "$ stdbuf -o0 cat {} > {}".format(log_port, log_file),
            flush=True,
        )
    elif capture_before_flash:
        capture = SerialLogCapture(log_port, args.baud, log_file)
        capture.start()

    flash_env = os.environ.copy()
    flash_env["BAUD"] = str(args.baud)

    try:
        if not args.skip_runtime_flash:
            run(
                [str(runtime_script), str(flash_port)],
                application,
                args.dry_run,
                flash_env,
            )
        if not args.skip_app_flash:
            run(
                [str(app_script), str(flash_port)],
                application,
                args.dry_run,
                flash_env,
            )

        if not args.skip_log and not args.dry_run:
            if capture is None:
                capture = SerialLogCapture(log_port, args.baud, log_file)
                capture.start()
            capture.wait(args.log_seconds)
    finally:
        if capture is not None:
            capture.stop()

    if not args.skip_log and not args.dry_run:
        check_log(log_file, args.expect, args.reject)
        print("Serial log: {}".format(log_file), flush=True)

    print("Embedded CI test completed.", flush=True)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\n[FAIL] Interrupted", file=sys.stderr)
        sys.exit(130)
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        print("[FAIL] {}".format(error), file=sys.stderr)
        sys.exit(1)
