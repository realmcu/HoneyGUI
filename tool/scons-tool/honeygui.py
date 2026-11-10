#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# honeygui.py

import os
from SCons.Script import Command

def bin2obj_action(target, source, env):
    """
    SCons Action to convert a binary file (.bin) to an object file (.o).
    """
    bin_dir = os.path.dirname(str(source[0]))
    bin_name = os.path.basename(str(source[0]))
    obj_name = os.path.basename(str(target[0]))
    
    # Construct the objcopy command
    cmd = f'cd "{bin_dir}" && objcopy -I binary -O elf64-x86-64 --binary-architecture i386 "{bin_name}" "{obj_name}"'
    
    print('[BIN2OBJ]', cmd)
    
    # Execute the command
    return os.system(cmd)

def add_prebuilt_binary_object(bin_file_path, obj_file_path):
    if os.path.exists(bin_file_path):
        bin_obj_node = Command(
            target=obj_file_path,
            source=bin_file_path,
            action=bin2obj_action
        )
        return bin_obj_node
    else:
        print(f"SCons-Warning: Prebuilt binary file not found and will be skipped: {bin_file_path}")
        return None