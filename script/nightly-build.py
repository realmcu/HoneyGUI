#!/usr/bin/env python

import sys
import git
import subprocess
import os
import re
import time
import glob
import argparse
import shutil
import importlib
from project_update import *
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


def gcc_build():
    try:
        os.chdir('./win32_sim')
        subprocess.check_call(["scons"], universal_newlines=True, stderr=subprocess.STDOUT)
        os.chdir('./..')
        subprocess.check_call([".\\win32_sim\\gui.exe"], universal_newlines=True, stderr=subprocess.STDOUT)
    except Exception as e:
        print("gcc build error: {}".format(e))


def keil_scons(repo):
    change_or_revert_macros(repo, "./keil_sim/menu_config.h", "change", [("BUILD_USING_SCRIPT_AS_A_APP", "", "BUILD_USING_SCRIPT_AS_A_APP")], True)
    os.chdir('./keil_sim')
    subprocess.check_call(["scons", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
    os.chdir('./..')


def send_mail(err_msg = None, attachment_file = None):
    sys.path.append(r'D:\admin\DependTools\Python Lib\MailSending')
    from mail import MailSending
    send_mail = MailSending()
    send_mail.empty_sending_mail_info()
    send_mail.set_mail_from('bluetooth_jenkins <bluetooth_jenkins@realsil.com.cn>')
    send_mail.add_mail_cc(['howie_wang@realsil.com.cn', 'wenjing_jiang@realsil.com.cn', 'tracy_yan@realsil.com.cn'])
    if err_msg or attachment_file:
        send_mail.set_mail_subject('HoneyGUI gui-engine-test-chip nightly build all project fail')
        mail_content = "JenkinsLink: {}<br>".format(os.getenv("BUILD_URL"))
        send_mail.set_mail_hightlight(True)
        if err_msg:
            mail_content += "{}<br>".format(err_msg)
        if attachment_file:
            send_mail.add_mail_attachment(attachment_file)
        send_mail.set_mail_content(mail_content, True)
    else:
        send_mail.set_mail_subject('HoneyGUI gui-engine-test-chip nightly build all project pass')
    send_mail.send_mail()
    send_mail.quit()


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
    #scons
    os.chdir('./win32_sim')
    try:
        subprocess.check_call(["scons.exe"], universal_newlines=True, stderr=subprocess.STDOUT)
    except Exception as e:
        os.chdir('./..')
        send_mail("win32_sim: 'scons' fail.", None)
        sys.exit("win32_sim: 'scons' fail, {}".format(e))
    os.chdir('./..')
    #reset
    repo.git.checkout('--', '.')
    repo.git.clean('-dfx')
    #scons --target=mdk5
    change_or_revert_macros(repo, "./keil_sim/menu_config.h", "change", [("BUILD_USING_SCRIPT_AS_A_APP", "", "BUILD_USING_SCRIPT_AS_A_APP")], True)
    os.chdir('./keil_sim')
    try:
        subprocess.check_call(["scons.exe", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
    except Exception as e:
        os.chdir('./..')
        send_mail("keil_sim: 'scons --target=mdk5' after enable BUILD_USING_SCRIPT_AS_A_APP fail.", None)
        sys.exit("keil_sim: 'scons --target=mdk5' after enable BUILD_USING_SCRIPT_AS_A_APP fail, {}".format(e))
    os.chdir('./..')
    #reset
    repo.git.checkout('--', '.')
    repo.git.clean('-dfx')
    #keil
    if not keil_builder.build_all_keil_projects(all=True, fail_fast=True, keil_path=os.environ.get("Keil_Path"), error_record_path=os.path.join(os.getcwd(), "error_record_{}".format(chip_type))):
        log_file = sorted(glob.glob(os.path.join(os.path.join(os.getcwd(), "error_record_{}".format(chip_type)), '*.txt')), reverse=True)[0]
        send_mail("keil_sim: keil build fail", log_file)
        sys.exit("build {} fail".format(chip_type))