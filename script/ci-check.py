import os
import sys
import argparse
import git
import subprocess


honeyRepo_env = "HoneyRepo"
honeyRepo_uitl_dir = os.path.join(os.environ.get(honeyRepo_env), "build", "utils")

print("repo home: ", os.environ[honeyRepo_env])
print("honeyRepo_uitl_dir", honeyRepo_uitl_dir)
print("jenkins base", os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))

sys.path.append(honeyRepo_uitl_dir)
sys.path.append(os.path.join(os.environ[honeyRepo_env], "build", "jenkins"))

from ci_check import *
from jenkins_base import JenkinsCheckBase

class CICheck(CICheckBase):
    pass


if __name__ == '__main__':
    ci_check = CICheck()
    ci_check.do_ci_check()

