#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
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
from helper.keil_helper import *
from project_update import *
from build.sdk_build import *

west_home = os.environ.get("WestHome")
jenkinsBuild_home = os.environ.get("jenkinsScript_abspath")
JenkinsBuild_util_Dir = os.path.join(jenkinsBuild_home, "utils")
JenkinsBuild_jenkins_Dir = os.path.join(jenkinsBuild_home, "jenkins")
current_prj_path = os.environ.get("current_prj_abspath")
current_prj_name = os.environ.get("current_prj_name")
realtek_app_path = os.path.join(west_home, r"zephyrproject/modules/honeygui")
yaml_path = os.environ.get("yaml_abs_path")

print("west_home: ", west_home)
print("JenkinsBuild_util_Dir", JenkinsBuild_util_Dir)
print("jenkins base", JenkinsBuild_jenkins_Dir)
print("subGitPath", current_prj_path)

sys.path.append(JenkinsBuild_util_Dir)
sys.path.append(JenkinsBuild_jenkins_Dir)
from repo_helper import *
from jenkins_base import JenkinsCheckBase
from keil_build import *

Keil_path = os.environ.get("Keil_Path")

def SDK_handler(module, submodule, yaml_path, west_home, chip_type):
    global Keil_path
    repo = git.Repo(search_parent_directories=True)
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    #simulator/win32 scons
    os.chdir('./simulator/win32')
    try:
        result_lines = subprocess.check_output(["scons.exe"], universal_newlines=True, stderr=subprocess.STDOUT)
        print("./simulator/win32 > scons.exe\r\n{}".format(result_lines))
        if 'warning:' in result_lines:
            os.chdir('./../..')
            print(result_lines)
            print("simulator/win32: 'scons' fail: has warning")
            return False
    except Exception as e:
        os.chdir('./../..')
        print("simulator/win32: 'scons' fail: {}".format(e.output))
        return False
    os.chdir('./../..')
    #reset
    repo.git.checkout('--', '.')
    repo.git.clean('-dfx')
    #lib scons --target=mdk5
    '''os.chdir('./lib')
    try:
        result_lines = subprocess.check_output(["scons.exe", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
        print("./lib > scons.exe\r\n{}".format(result_lines))
        if 'warning:' in result_lines:
            os.chdir('./..')
            print(result_lines)
            print("lib: 'scons --target=mdk5' fail: has warning")
            return False
    except Exception as e:
        os.chdir('./..')
        print("lib: 'scons --target=mdk5' fail: {}".format(e.output))
        return False
    os.chdir('./..')'''
    #reset
    '''repo.git.checkout('--', '.')
    repo.git.clean('-dfx')'''
    #simulator/keil/ac5 scons --target=mdk5
    os.chdir('./simulator/keil/ac5')
    try:
        result_lines = subprocess.check_output(["scons.exe", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
        print("./simulator/keil/ac5 > scons.exe\r\n{}".format(result_lines))
        if 'warning:' in result_lines:
            os.chdir('./../../..')
            print(result_lines)
            print("simulator/keil/ac5: 'scons --target=mdk5' fail: has warning")
            return False
    except Exception as e:
        os.chdir('./../../..')
        print("simulator/keil/ac5: 'scons --target=mdk5' fail: {}".format(e.output))
        return False
    os.chdir('./../../..')
    #reset
    '''repo.git.checkout('--', '.')
    repo.git.clean('-dfx')'''
    #simulator/keil/ac6 scons --target=mdk5
    os.chdir('./simulator/keil/ac6')
    try:
        result_lines = subprocess.check_output(["scons.exe", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
        print("./simulator/keil/ac6 > scons.exe\r\n{}".format(result_lines))
        if 'warning:' in result_lines:
            os.chdir('./../../..')
            print(result_lines)
            print("simulator/keil/ac6: 'scons --target=mdk5' fail: has warning")
            return False
    except Exception as e:
        os.chdir('./../../..')
        print("simulator/keil/ac6: 'scons --target=mdk5' fail: {}".format(e.output))
        return False
    os.chdir('./../../..')
    #reset
    '''repo.git.checkout('--', '.')
    repo.git.clean('-dfx')'''
    #build keil
    keil_builder = SDKBuild(yaml_path, west_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.build_all_keil_projects(title=module, all=True, fail_fast=True, keil_path=Keil_path):
        print("build {} fail".format(chip_type))
        return False
    #reset
    repo.git.checkout('--', '.')
    repo.git.clean('-dfx')

    return True


def Test_handler(module, submodule, yaml_path, west_home, chip_type):
    os.chdir('./example/ci_build_scripts')
    try:
        result_lines = subprocess.check_output([sys.executable, r'all_build.py'], universal_newlines=True, stderr=subprocess.STDOUT)
        print('all_build.py\r\n{}'.format(result_lines))
        if 'FINAL RESULT: [FAIL] SOME TESTS FAILED' in result_lines or 'FINAL RESULT: [OK] ALL TESTS PASSED' not in result_lines:
            print("all_build fail")
            return False
    except Exception as e:
        print("all_build.py fail: {}".format(e.output))
        return False
    os.chdir('./../..')
    return True


def DOC_handler(module, submodule, yaml_path, west_home, chip_type):
    print("build sphinx document\n")
    try:
        cmd = [sys.executable, os.path.join(os.path.dirname(os.path.abspath(__file__)), "../doc/build.py")]
        print(" ".join(cmd), flush=True)
        subprocess.check_call(cmd, universal_newlines=True, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print("Generate sphinx document exception: {}".format(e))
        return False
    return True


def GUI_handler(module, submodule, yaml_path, west_home, chip_type):
    ret1 = SDK_handler(module, submodule, yaml_path, west_home, chip_type)
    if not ret1:
        print("SDK build fail!")
        return False
    ret2 = DOC_handler(module, submodule, yaml_path, west_home, chip_type)
    if not ret2:
        print("DOC build fail!")
        return False
    ret3 = Test_handler(module, submodule, yaml_path, west_home, chip_type)
    if not ret3:
        print("Test build fail!")
        return False
    return True


def GUI_No_Test_handler(module, submodule, yaml_path, west_home, chip_type):
    ret1 = SDK_handler(module, submodule, yaml_path, west_home, chip_type)
    if not ret1:
        print("SDK build fail!")
        return False
    ret2 = DOC_handler(module, submodule, yaml_path, west_home, chip_type)
    if not ret2:
        print("DOC build fail!")
        return False
    return True


def App_handler(module, submodule, yaml_path, west_home, chip_type):
    print("app handler")
    global Keil_path
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    keil_builder = SDKBuild(yaml_path, west_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.get_keyword_project_list(exact=True, folder=submodule, ignore_warning=True, error_record_path="", start_from_last_fail=True, only_build_app=False,keil_path=Keil_path):
        print("build {} fail".format(chip_type))
        return False

    return True


def Debug_handler(module, submodule, yaml_path, west_home, chip_type):
    print('Bypass Module \'{}\''.format(module), flush=True)
    return False


def Keyword_handler(module, submodule, yaml_path, west_home, chip_type):
    print("Keyword handler")
    global Keil_path
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    keil_builder = SDKBuild(yaml_path, west_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.get_keyword_project_list(exact=False, folder=submodule, ignore_warning=True, error_record_path="", start_from_last_fail=True, only_build_app=False,keil_path=Keil_path):
        print("build {} fail".format(chip_type))
        return False

    return True


# module tuple table: module, submodule, handler
module_table =  (
                    ['honeygui', 'GUI',          '',              GUI_handler ],
                    ['honeygui', 'GUI',          'BypassTest',    GUI_No_Test_handler ],
                    ['honeygui', 'Doc',          '',              DOC_handler ],

                    ['honeygui', 'Script',       'CI',            GUI_handler ],
                    ['honeygui', 'Script',       'Other',         SDK_handler ],
                    ['honeygui', 'Script',       'Nightly',       SDK_handler ],
                    ['honeygui', 'Script',       'Release',       SDK_handler ],

                    ['honeygui', 'Debug',        '',              Debug_handler ],
            )


class CIBuild(JenkinsCheckBase):
    # subgit_repo_path: the project trigger the jenkins
    def __init__(self, subgit_repo_path, subgit_group_name, yaml_path, west_home, **kwargs):
        self.subgit_repo_path = subgit_repo_path
        self.subgit_repo = git.Repo(path=subgit_repo_path)

        self.subgit_group_name = subgit_group_name
        self.yaml_path = yaml_path
        self.west_home = west_home
        self.chip_type = kwargs["chip_type"]


    def get_module_submodule_handler_from_commit_message(self, module_table):
        commit_title = self.subgit_repo.git.log('-1', 'HEAD', "--pretty='%s'").strip('\'')
        commit_head = commit_title.lower().split(':')[0].rstrip()
        sub_head = commit_head.split('-')[-1]
        if commit_head.split("-")[0] in ["app","keyword"]:
            commit_head = "{}-*".format(commit_head.split("-")[0])
        for (subgit_group_name, module, submodule, handler) in module_table:
            prefix = '-'.join([module, submodule]).rstrip('-')
            if commit_head == prefix.lower():
                if subgit_group_name == self.subgit_group_name:
                    return subgit_group_name, module, sub_head, handler
        return None, None, None, None


    def build_handler(self):
        commit_title = self.subgit_repo.git.log('-1', 'HEAD', "--pretty='%s'").strip('\'')
        group_name, module, submodule, handler = self.get_module_submodule_handler_from_commit_message(module_table)
        if not handler(module, submodule, self.yaml_path, self.west_home, self.chip_type):
            self.gerrit_comment_wrap({}, label_name="Build-Verified", result=False, write_flag = True, message="Build project fail, refer to {}console for detail".format(os.environ['BUILD_URL']))
            return False
        return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='HoneyPatch CI build script')
    parser.add_argument('-c', '--chipType', help='Set the chip type')
    arg_dict = parser.parse_args()

    ci_build = CIBuild(subgit_repo_path=current_prj_path, 
            subgit_group_name=current_prj_name,
            yaml_path=yaml_path,
            west_home=west_home,
            chip_type = arg_dict.chipType)
    if not ci_build.build_handler():
        sys.exit(1)