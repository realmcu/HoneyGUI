#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import sys
import log
import glob
import shutil
from urllib.parse import quote
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from base.parse_json import ParseJson
from ftp.ftp_helper import *


def find_report(src_path):
    report_list = list()
    print("Find hrl report in {}".format(src_path))
    for dir_path, _ , file_names in os.walk(src_path):
        for file_name in file_names:
            if file_name.endswith(r'.xls') or file_name.endswith(r'.xlsx') and "logSequence" in file_name:
                print("find ", dir_path, file_name)
                report_list.append(os.path.join(dir_path, file_name))
    print("Find report list: {}".format(report_list))
    return report_list


def get_jira_info_json_name(module):
    jira_info_name = 'jira_info~{}.json'.format(module)
    return jira_info_name


def copy_report(project,  module, sequence_index, test_type, is_dryrun, compiler, log_path):
    #copy hrs report into report folder
    report_path = os.path.join(os.environ.get("WORKSPACE"), r'SRC\report')
    os.makedirs(report_path, exist_ok=True)
    nodeName = os.environ.get("NODE_NAME")

    abs_log_path = "log"
    test_info_path = os.path.join(r"{}".format(module))
    ftp_log_path = os.path.join(quote(get_ftp_folder()[0], "utf-8"), test_info_path, abs_log_path)
    print("log to {}".format(os.path.join(test_info_path, abs_log_path)))

    try:
        if module in ["HoneyGUI_CMD"]:
            hrs_report_path = report_path
            hrs_report_list = glob.glob(os.path.join(report_path, "*.xlsx"))
        else:
            hrs_report_path = os.path.join(log_path, r'HRL')
            hrs_report_list = find_report(hrs_report_path)

        print("HRS report path: {}, hrs_report: {}".format(hrs_report_path, hrs_report_list))
    except Exception as e:
        print("read hrs report info error: {}".format(e))

    report_xml_file = "report_xml_map.json"
    report_xml_map = dict()
    if os.path.exists(os.path.join(report_path, report_xml_file)):
        report_xml_map = ParseJson.load_json_file(report_xml_file, dir_name = report_path)
    hrs_report_list = hrs_report_list or [""]
    for hrs_report in hrs_report_list:
        report_config = {}
        report_config = write_report_config(report_config, os.environ.get("Test_Result", "FAILURE"),
                                            module, nodeName, ftp_log_path, 
                                            test_info_path, compiler)
        report_config["report_name"] = hrs_report
        report_config["xml_name"] = report_xml_map.get(hrs_report, "")
        try:
            jira_info_name = get_jira_info_json_name(module)
            ParseJson.write_json_file(json_content=report_config, file_name=jira_info_name, dir_name=report_path)
        except Exception as e:
            print(e)
            continue


def write_report_config(report_config, result, module, nodeName, ftp_log_path, test_info_path, compiler):
    test_info_path = os.path.join(r"{}".format(module))
    abs_log_path = "log"
    ftp_log_path = os.path.join(quote(get_ftp_folder()[0], "utf-8"), test_info_path, abs_log_path)
    HTTP_LINK  = r"http://{0}:5000/index.cgi?launchApp=SYNO.SDS.App.FileStation3.Instance&launchParam=openfile"
    report_config["compiler"] = compiler
    report_config["test_module"] = module
    report_config["node_name"] = nodeName
    report_config["module_name"] = module
    report_config["result"] = result
    report_config["url"] = os.getenv("BUILD_URL")
    report_config["log_path"] = HTTP_LINK.format(get_ftp_host()[0][0]) + quote("={}/".format(ftp_log_path.replace("\\", "/")), "utf-8")
    report_config["bin_path"] = HTTP_LINK.format(rs_ftp_host()[0]) + quote("={}/".format(os.path.join(quote(rs_ftp_server(), "utf-8"), test_info_path, "bin").replace("\\", "/")), "utf-8")

    return report_config

