#!/usr/bin/env python

import sys
import git
import subprocess
import os
import re
import time
import argparse
import shutil
import importlib
from build.sdk_build import *

honeyRepo_env = "HoneyRepo"
manifest_path_env = "manifest"

honeyRepo_uitl_dir = os.path.join(os.environ.get(honeyRepo_env), "build", "utils")

print("repo home: ", os.environ[honeyRepo_env])
print("honeyRepo_uitl_dir", honeyRepo_uitl_dir)
print("jenkins base", os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))
print(os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))

sys.path.append(os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))
sys.path.append(honeyRepo_uitl_dir)

from repo_helper import *
from jenkins_base import JenkinsCheckBase
from keil_build import *


def change_or_revert_macros(repo, app_flag_path, update_type, macro_list, insert=False):
    if update_type == "change" and not macro_list:
        print("There is no macro to change, skip")
        return

    if update_type == 'change':
        with open(app_flag_path, mode='r', newline='', errors='surrogateescape') as fd:
            lines = fd.readlines()
 
        with open(app_flag_path, mode='w+', newline='', errors='surrogateescape') as fd:
            for macro, value, check_pattern in macro_list:
                start_replace = False if check_pattern else True
                pattern = r'^(\s*#define\s+{}\s+)(\d+)'.format(macro)
                for i, line in enumerate(lines):
                    if check_pattern and re.search(check_pattern, line):
                        print("Find target definition, start change macro {}".format(macro))
                        start_replace = True
                    if start_replace:
                        if insert:
                            lines[i] = line + "#undef {}\n#define {}\t{}\n".format(macro, macro, value)
                            start_replace = False
                        else:
                            if re.search(pattern, line):
                                lines[i] = re.sub(pattern, lambda objs:objs.group(1)+value, line, count=1, flags=re.M)
                                start_replace = False
            fd.writelines(lines)
        
        print('Configure {} diff ->\n{}'.format(os.path.basename(app_flag_path), repo.git.diff(app_flag_path)), flush=True)
        
    elif update_type == 'revert':
        repo.git.checkout('--', app_flag_path)
        print('after revert {} diff ->\n{}'.format(os.path.basename(app_flag_path), repo.git.diff(app_flag_path)), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='HoneyPatch CI build script')
    parser.add_argument('-c', '--chipType', choices=['guiEngine-test-chip'], help='Set the chip type')
    arg_dict = parser.parse_args()
    chip_type = arg_dict.chipType

    repo = git.Repo(search_parent_directories=True)
    repo.git.clean('-dfx')
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    keil_builder = SDKBuild(os.environ.get("manifest"), os.environ.get("HoneyRepo"), chip_type)
    print("call build {}".format(chip_type))
    change_or_revert_macros(repo, "./keil_sim/menu_config.h", "change", [("BUILD_USING_SCRIPT_AS_A_APP", "", "BUILD_USING_SCRIPT_AS_A_APP")], True)
    os.chdir('./keil_sim')
    subprocess.check_call(["scons", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
    os.chdir('./..')
    if not keil_builder.build_all_keil_projects(all=True, fail_fast=True, keil_path=os.environ.get("Keil_Path")):
        sys.exit("build {} fail".format(chip_type))