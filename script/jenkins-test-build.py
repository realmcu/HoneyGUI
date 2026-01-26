#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
#!/usr/bin/env python3

import os
import sys
import re
import subprocess
import shutil
import git
import argparse
import glob
import xml.etree.ElementTree as ET
import json
import importlib
from datetime import datetime
from ftp.copy_image import CopyImage


def get_file_single(pathname):
    file_matches = glob.glob(pathname)
    if len(file_matches)!= 1:
        print('only 1 {} file for should be existed, now {} image(s) existed'.format(pathname, len(file_matches)))
        return None
    return file_matches[0]

config_path = r'win32_sim/.config'
def HoneyGUI_CMD_preBuild():
    with open(os.path.join(HONEYGUI_ROOT, config_path), mode='r', newline='', errors='surrogateescape') as fd:
        stream = fd.read()
    # Disable current demo
    stream = re.sub(r'^(CONFIG_REALTEK_BUILD_GUI_454_454_DEMO=y)', r'# \1 is not set', stream, count=1, flags=re.M)
    # Enable test demos
    stream = re.sub(r'^# (CONFIG_REALTEK_BUILD_HONEYGUI_AUTO_TEST) is not set', r'\1=y', stream, count=1, flags=re.M)
    stream = re.sub(r'^# (CONFIG_REALTEK_BUILD_GUI_410_502_DEMO) is not set', r'\1=y', stream, count=1, flags=re.M)
    stream = re.sub(r'^# (CONFIG_REALTEK_BUILD_GUI_800_480_DEMO) is not set', r'\1=y', stream, count=1, flags=re.M)
    with open(os.path.join(HONEYGUI_ROOT, config_path), mode='w+', newline='', errors='surrogateescape') as fd:
        fd.write(stream)
    print(f"git diff {config_path}")
    subprocess.check_call(["git", "diff", config_path], cwd=HONEYGUI_ROOT)


def HoneyGUI_CMD_afterBuild():
    subprocess.check_call(["git", "checkout", "--", config_path], cwd=HONEYGUI_ROOT)
    print(f"after checkout {config_path}")
    subprocess.check_call(["git", "diff", config_path], cwd=HONEYGUI_ROOT)


HoneyGUI_build_hook = {
    "HoneyGUI_CMD": {
        "PreBuild": HoneyGUI_CMD_preBuild,
        "AfterBuild": HoneyGUI_CMD_afterBuild
    }
}


def build_honeyGUI(build_type, module_paratmeters):
    for module_name in module_paratmeters:
        if not module_paratmeters[module_name]:
            continue

        if HoneyGUI_build_hook.get(module_name, {}).get("PreBuild"):
            HoneyGUI_build_hook.get(module_name, {}).get("PreBuild")()
        subprocess.check_call(['scons'], universal_newlines=True, cwd=os.path.join(HONEYGUI_ROOT, "win32_sim"))
        if HoneyGUI_build_hook.get(module_name, {}).get("AfterBuild"):
            HoneyGUI_build_hook.get(module_name, {}).get("AfterBuild")()

        gui_exe = get_file_single(os.path.join(HONEYGUI_ROOT, "win32_sim", r'gui.exe'))
        gui_bin = get_file_single(os.path.join(HONEYGUI_ROOT, "win32_sim", r'gui.bin'))
        if not gui_exe or not gui_bin:
            print("build gui.exe fail")
            sys.exit(1)
        bin_relative_dir = f"{module_name}/bin"
        bin_dir = os.path.join(build_dict_dir, bin_relative_dir)
        os.makedirs(bin_dir, exist_ok=True)
        shutil.copy2(gui_exe, bin_dir)
        shutil.copy2(gui_bin, bin_dir)
        shutil.copy2(os.path.join(HONEYGUI_ROOT, "win32_sim", "SDL2.dll"), bin_dir)
        migwin_root = os.path.dirname(shutil.which("gcc"))
        migwin_root = os.path.dirname(migwin_root)
        lib_root = os.path.join(migwin_root, "x86_64-w64-mingw32\lib")
        shutil.copy2(os.path.join(lib_root, "libstdc++-6.dll"), bin_dir)
        shutil.copy2(os.path.join(lib_root, "libgcc_s_sjlj-1.dll"), bin_dir)
        shutil.copy2(os.path.join(lib_root, "libwinpthread-1.dll"), bin_dir)

        CopyImage.copy_image_to_ftp(build_dict_dir, build_dict_dir, os.path.dirname(bin_relative_dir), module_name)


def clean_repo(repo_name):
    repo = git.Repo(search_parent_directories=True)
    try:
        repo.git.rev_parse('--verify', 'HEAD')
        repo.git.clean('-dfx')
        repo.git.reset('--hard')
        print("Clean repo {}".format(repo_name))
    except (git.exc.GitCommandError, ValueError, IndexError) as e:
        sys.exit('Error empty repo {}'.format(e))


def get_repo_commitID():
    commitID = subprocess.check_output([r'git', r'log', "-1", "HEAD", "--pretty=%H"], universal_newlines=True)
    commitMsg = subprocess.check_output([r'git', r'log', "-1", "HEAD", "--pretty=%s"], universal_newlines=True)
    return commitID, commitMsg


def check_repo(repo_name):
    clean_repo(repo_name)
    commitID, commitMsg = get_repo_commitID(repo_name)
    print("[{}] {} {}".format(repo_name, commitID, commitMsg))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='bb2-sdk-v1 test build script')
    parser.add_argument('buildType', choices=['CI', 'Nightly', 'Release', 'FreeStyle'], help='set the build tpye')
    parser.add_argument('--triggeredBy', default='gerrit', choices=['gerrit', 'USER', 'TIMER', 'SCM','HRL_Tool_CI','HRL_Tool_Release', "HRS_Tool_CI", "HRS_Tool_Release"])

    parser.add_argument('--binDir', help='build output dir')
    parser.add_argument('--HoneyGUI_CMD', action = 'store_true', help = 'is need to test HoneyGUI_CMD')
    parser.add_argument('--HoneyGUI_Simulator', action = 'store_true', help='is need to test HoneyGUI_Simulator')


    arg_dict = parser.parse_args()
    build_type = arg_dict.buildType
    triggered_by = arg_dict.triggeredBy


    # HoneyPatch Repo root directory
    HONEYGUI_ROOT = os.environ['HONEYGUI_ROOT']

    module_input_parameters = {}
    for m in ["HoneyGUI_CMD", "HoneyGUI_Simulator"]:
        module_input_parameters[m] = getattr(arg_dict, m)

    build_dict_dir = os.path.join(os.environ.get("WORKSPACE"), arg_dict.binDir)

    if os.path.exists(build_dict_dir):
        shutil.rmtree(build_dict_dir)
    os.makedirs(build_dict_dir)

    build_honeyGUI(build_type=build_type,
                   module_paratmeters=module_input_parameters)

