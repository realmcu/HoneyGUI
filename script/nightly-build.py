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
    if not keil_builder.build_all_keil_projects(all=True, fail_fast=True, keil_path=os.environ.get("Keil_Path")):
        sys.exit("build {} fail".format(chip_type))