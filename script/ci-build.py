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
sub_git_group_env = 'subGitGroup'
sub_git_path_env = 'subGitPath'


honeyRepo_uitl_dir = os.path.join(os.environ.get(honeyRepo_env), "build", "utils")

print("repo home: ", os.environ[honeyRepo_env])
print("honeyRepo_uitl_dir", honeyRepo_uitl_dir)
print("jenkins base", os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))
print("subGitPath", os.environ.get("subGitPath", ""))
print(os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))

sys.path.append(os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))
sys.path.append(honeyRepo_uitl_dir)

from repo_helper import *
from jenkins_base import JenkinsCheckBase
from keil_build import *

Keil_path = r'D:\Keil_v5_536\UV4\uv4.exe'
if not os.path.exists(Keil_path):
    Keil_path = r'C:\Keil_v5.36\UV4\uv4.exe'
def SDK_handler(module, submodule, manifest_path, repo_home, chip_type):
    global Keil_path
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    keil_builder = SDKBuild(manifest_path, repo_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.build_all_keil_projects(all=True, fail_fast=True, keil_path=Keil_path):
        print("build {} fail".format(chip_type))
        return False

    return True


def App_handler(module, submodule, manifest_path, repo_home, chip_type):
    print("app handler")
    global Keil_path
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    keil_builder = SDKBuild(manifest_path, repo_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.get_keyword_project_list(exact=True, folder=submodule, ignore_warning=True, error_record_path="", start_from_last_fail=True, only_build_app=False,keil_path=Keil_path):
        print("build {} fail".format(chip_type))
        return False

    return True


def Debug_handler(module, submodule, manifest_path, repo_home, chip_type):
    print('Bypass Module \'{}\''.format(module), flush=True)
    return False


def Keyword_handler(module, submodule, manifest_path, repo_home, chip_type):
    print("Keyword handler")
    global Keil_path
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    keil_builder = SDKBuild(manifest_path, repo_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.get_keyword_project_list(exact=False, folder=submodule, ignore_warning=True, error_record_path="", start_from_last_fail=True, only_build_app=False,keil_path=Keil_path):
        print("build {} fail".format(chip_type))
        return False

    return True


# module tuple table: module, submodule, handler
module_table =  (
                    ['sdk', 'GUI',          '',         SDK_handler ],

                    ['sdk', 'Script',       'CI',            SDK_handler ],
                    ['sdk', 'Script',       'Other',         SDK_handler ],
                    ['sdk', 'Script',       'Nightly',       SDK_handler ],
                    ['sdk', 'Script',       'Release',       SDK_handler ],

                    ['sdk', 'Debug',        '',       Debug_handler ],
            )


class CIBuild(JenkinsCheckBase):
    # subgit_repo_path: the project trigger the jenkins
    def __init__(self, subgit_repo_path, subgit_group_name, manifest_path, repo_home, **kwargs):
        self.subgit_repo_path = subgit_repo_path
        self.subgit_repo = git.Repo(path=subgit_repo_path)

        self.subgit_group_name = subgit_group_name
        self.manifest_path = manifest_path
        self.repo_home = repo_home
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
        if not handler(module, submodule, self.manifest_path, self.repo_home, self.chip_type):
            self.gerrit_comment_wrap({}, label_name="Build-Verified", result=False, write_flag = True, message="Build project fail, refer to {}console for detail".format(os.environ['BUILD_URL']))
            return False
        return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='HoneyPatch CI build script')
    parser.add_argument('-c', '--chipType', choices=['guiEngine-dev'], help='Set the chip type')
    arg_dict = parser.parse_args()

    ci_build = CIBuild(subgit_repo_path=os.environ.get(sub_git_path_env), 
            subgit_group_name=os.environ.get(sub_git_group_env),
            manifest_path=os.environ.get(manifest_path_env),
            repo_home=os.environ.get(honeyRepo_env),
            chip_type = arg_dict.chipType)
    if not ci_build.build_handler():
        sys.exit(1)
