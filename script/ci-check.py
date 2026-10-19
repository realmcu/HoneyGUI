#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import sys
import argparse
import git
import subprocess

west_home = os.environ.get("WestHome")
jenkinsBuild_home = os.environ.get("jenkinsScript_abspath")
JenkinsBuild_util_Dir = os.path.join(jenkinsBuild_home, "utils")
JenkinsBuild_jenkins_Dir = os.path.join(jenkinsBuild_home, "jenkins")

print("west home: ", west_home)
print("JenkinsBuild_util_Dir", JenkinsBuild_util_Dir)
print("jenkins base", JenkinsBuild_jenkins_Dir)

sys.path.append(JenkinsBuild_util_Dir)
sys.path.append(JenkinsBuild_jenkins_Dir)
from ci_check import *

def doc_handler(gitobj):
    pass

class CICheck(CICheckBase):
    pass


if __name__ == '__main__':
    ci_check = CICheck()
    ci_check.do_ci_check()
