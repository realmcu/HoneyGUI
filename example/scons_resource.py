#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import subprocess

def _bin2obj_action(target, source, env):
    input_file = os.path.abspath(str(source[0]))
    output_file = os.path.abspath(str(target[0]))
    output_format = (
        "pe-x86-64" if env["TARGET_OS"] == "windows" else "elf64-x86-64"
    )
    objcopy = env.WhereIs(env["OBJCOPY"]) or env["OBJCOPY"]

    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    return subprocess.call(
        [
            objcopy,
            "-I",
            "binary",
            "-O",
            output_format,
            "--binary-architecture",
            "i386:x86-64",
            os.path.basename(input_file),
            output_file,
        ],
        cwd=os.path.dirname(input_file),
        env=env.get("ENV"),
    )


def add_prebuilt_binary_object(env, bin_file_path, obj_file_path):
    if not os.path.exists(bin_file_path):
        print(
            "SCons-Warning: Prebuilt binary file not found and will be skipped: "
            + bin_file_path
        )
        return None

    return env.Command(
        target=obj_file_path,
        source=bin_file_path,
        action=_bin2obj_action,
    )
