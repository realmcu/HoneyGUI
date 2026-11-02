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
import glob
import argparse
import shutil
import importlib

from project_update import *
from build.sdk_build import *

west_home = os.environ.get("WestHome")
jenkinsBuild_home = os.environ.get("jenkinsScript_abspath")
JenkinsBuild_util_Dir = os.path.join(jenkinsBuild_home, "utils")
JenkinsBuild_jenkins_Dir = os.path.join(jenkinsBuild_home, "jenkins")
current_prj_path = os.environ.get("current_prj_abspath")
current_prj_name = os.environ.get("current_prj_name")
honeygui_path = os.path.join(west_home, r"zephyrproject/modules/honeygui")
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


def send_mail(err_msg = None, attachment_file = None):
    sys.path.append(r'D:\admin\DependTools\Python Lib\MailSending')
    from mail import MailSending
    send_mail = MailSending()
    send_mail.empty_sending_mail_info()
    send_mail.set_mail_from('bluetooth_jenkins <bluetooth_jenkins@realsil.com.cn>')
    send_mail.add_mail_cc(['howie_wang@realsil.com.cn', 'wenjing_jiang@realsil.com.cn', 'tracy_yan@realsil.com.cn'])
    if err_msg or attachment_file:
        send_mail.set_mail_subject('HoneyGUI guiEngine-master nightly build all project fail')
        mail_content = "JenkinsLink: {}<br>".format(os.getenv("BUILD_URL"))
        send_mail.set_mail_hightlight(True)
        if err_msg:
            mail_content += "{}<br>".format(err_msg)
        if attachment_file:
            send_mail.add_mail_attachment(attachment_file)
        send_mail.set_mail_content(mail_content, True)
    else:
        send_mail.set_mail_subject('HoneyGUI guiEngine-master nightly build all project pass')
    send_mail.send_mail()
    send_mail.quit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='HoneyGUI nightly build script')
    parser.add_argument('-c', '--chipType', help='Set the chip type')
    arg_dict = parser.parse_args()
    chip_type = arg_dict.chipType

    repo = git.Repo(search_parent_directories=True)
    repo.git.clean('-dfx')
    print("\n================ build {} ====================\n".format(chip_type), flush=True)
    #simulator/win32 scons
    os.chdir('./simulator/win32')
    try:
        subprocess.check_call(["scons.exe"], universal_newlines=True, stderr=subprocess.STDOUT)
    except Exception as e:
        os.chdir('./../..')
        send_mail("simulator/win32: 'scons' fail.", None)
        sys.exit("simulator/win32: 'scons' fail, {}".format(e))
    os.chdir('./../..')
    #reset
    repo.git.checkout('--', '.')
    repo.git.clean('-dfx')
    #simulator/keil/ac5 scons --target=mdk5
    os.chdir('./simulator/keil/ac5')
    try:
        subprocess.check_call(["scons.exe", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
    except Exception as e:
        os.chdir('./../../..')
        send_mail("simulator/keil/ac5: 'simulator/keil/ac5: 'scons --target=mdk5' fail.", None)
        sys.exit("simulator/keil/ac5: 'simulator/keil/ac5: 'scons --target=mdk5' fail, {}".format(e))
    os.chdir('./../../..')
    #reset
    '''repo.git.checkout('--', '.')
    repo.git.clean('-dfx')'''
    #simulator/keil/ac6 scons --target=mdk5
    os.chdir('./simulator/keil/ac6')
    try:
        subprocess.check_call(["scons.exe", "--target=mdk5"], universal_newlines=True, stderr=subprocess.STDOUT)
    except Exception as e:
        os.chdir('./../../..')
        send_mail("simulator/keil/ac6: 'simulator/keil/ac6: 'scons --target=mdk5' fail.", None)
        sys.exit("simulator/keil/ac6: 'simulator/keil/ac6: 'scons --target=mdk5' fail, {}".format(e))
    os.chdir('./../../..')
    #reset
    '''repo.git.checkout('--', '.')
    repo.git.clean('-dfx')'''
    #keil
    keil_builder = SDKBuild(yaml_path, west_home, chip_type)
    print("call build {}".format(chip_type))
    if not keil_builder.build_all_keil_projects(title="honeygui", all=True, fail_fast=True, keil_path=os.environ.get("Keil_Path"), error_record_path=os.path.join(os.getcwd(), "error_record_{}".format(chip_type))):
        log_file = sorted(glob.glob(os.path.join(os.path.join(os.getcwd(), "error_record_{}".format(chip_type)), '*.txt')), reverse=True)[0]
        send_mail("simulator/keil/ac6: keil build fail", log_file)
        sys.exit("build {} fail".format(chip_type))