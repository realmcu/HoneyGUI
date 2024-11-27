# -*- coding: utf-8 -*-
#!/usr/bin/env python
import argparse
from openpyxl import load_workbook
from openpyxl import Workbook
import os
import re
import time
import json
import sys
import glob
import xlrd
import shutil
import subprocess
import platform
import sqlite3
from openpyxl.styles import Border, Side, Alignment, PatternFill
import xlrd
from pathlib import Path
from urllib.parse import quote
from jenkinsapi.jenkins import Jenkins
from jenkinsapi.utils.crumb_requester import CrumbRequester
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from jenkins_test_target_config import *
import report.create_jira_config as create_jira_config


JIRA_ISSUE_REPORTER = r'bluetooth_jenkins'
HoneyGUI_OWNER = "liang_zhou"

BEEX_ERROR = r'BEEX_ERROR: Beex'
EXCEPTION_LOG = [r'found exception log', r'soc_action_fail: exception log found']

COMMON_URL = r'http://jenkins.realsil.com.cn/job/Dump~WaveFile/{0}/'
ARTIF_URL = r'artifact/dist/{0}'

WAVE_FILE = r'''WaveFile:
{0}
'''

ISSUE_DESC_COMMON = r'''
{0}第{1}次fail{2}
FailLoop: {3}
FailAction: {4}
'''

SPLIT_MARK = r'{color:red}******************{color}'

FREE_STYLE_COMMENT = r'''
*备注*:
1. Bug修正后, 可通过bb-freestyle验证或复现,链接：
{0}
2. 使用说明及网址: https://wiki.realtek.com/display/Bee1/FreeStyle+Test
3. 验证测试用例：{1}, 验证设置的压力测参数: cfg_LoopNumber = 500, cfg_maxFailNumber= 10; 或者根据自己的需求设定
4. 此jira最多只展示三次fail信息, 若要查看详细fail信息, 请访问Excel报告
'''

ISSUE_COMMENT = r'''
*测试模块&测试环境*: {0} & {1}
*HoneyGUI*: {2} {3}
*URL*: {4}
*LOG路径*: {5}
*测试用例 & 失败概率*: {6} & {7}
*问题说明*:
{8}'''


def connect_jira():
    func_name = 'connect_jira'
    user = os.getenv('JIRA_USR_NAME')
    password = os.getenv('JIRA_PWD')

    # Connect to JIRA server
    try:
        jira = JIRA(server='https://jira.realtek.com', basic_auth=(user, password))
    except Exception as e:
        sys.exit('{}: error connect server {}'.format(func_name, e))
    
    return jira


def read_test_info(test_info_file):
    test_info = {}
    if not os.path.exists(test_info_file):
        print("read_test_info: report config file doesn't exists", flush=True)
        return test_info
    try:
        with open(test_info_file) as f:
            test_info = json.load(f)
    except Exception as e:
        print('Load JSON file failed: {}'.format(e), flush=True)
        return test_info

    return test_info


def read_report(jira_info_dict, test_type, jira_info_file):
    report_file = jira_info_dict.get("report_name", "")
    if not os.path.exists(report_file):
        print("Cannot find specified file {}".format(report_file), flush=True)
        return
    workbook = load_workbook(report_file)
    if "TestReportBackUp" in workbook.get_sheet_names():
        return read_report_with_error_key(jira_info_dict, test_type, jira_info_file)
    else:
        return read_report_with_test_case(jira_info_dict, test_type, jira_info_file)


def read_excel(file_name,sheet=0):
    '''
    read excel, split merged cells
    :param file_name: excel file name
    :return: list
    '''
    report_list = []
    workbook = load_workbook(file_name)
    worksheet = workbook.worksheets[sheet]
    first_line = list(map(lambda x: x.value or '', worksheet[1]))
    report_list = list()
    merged_cells_data = dict()
    for merged_range in worksheet.merged_cells.ranges:
        min_row, min_col, max_row, max_col = merged_range.min_row, merged_range.min_col, merged_range.max_row, merged_range.max_col
        value_mg_cell = worksheet.cell(min_row, min_col).value
        for row in range(min_row, max_row + 1):
            for col in range(min_col, max_col + 1):
                merged_cells_data[(row, col)] = "" if value_mg_cell == None else value_mg_cell
    for row_idx in range(2, worksheet.max_row + 1):
        row_data = dict()
        for col_idx in range(1, worksheet.max_column + 1):
            if (row_idx, col_idx) in merged_cells_data:
                row_data[first_line[col_idx - 1]] = merged_cells_data[(row_idx, col_idx)]
            else:
                val = worksheet.cell(row=row_idx, column=col_idx).value
                row_data[first_line[col_idx - 1]] = "" if val == None else val
        report_list.append(row_data)
    
    return report_list


def add_format_for_fail_description(o_desc):
    o_desc = o_desc.replace("****************> ", "{color:red}****************> ").replace(" <****************", " <****************{color}")
    desc_list = o_desc.split("\n")
    i = -1
    need_end = False
    for c in desc_list:
        i += 1
        if c.startswith("FailAction:"):
            desc_list[i] = c + "\n{code:java}"
            need_end = True
            continue
        if need_end:
            if c.startswith("Below case has the same error key") or "***************>" in c:
                desc_list[i] = "{code}\n" + c
                need_end = False
                continue
    if need_end:
        desc_list.append("{code}\n")
    return "\n".join(desc_list)


def read_error_keys_with_error_key(report_list, is_freestyle=False):
    pattern_ignore_error = re.compile(r'\*IGNORE\*|\*SUCCESS\*.*')
    pattern_beex_error = re.compile(r'BEEX_ERROR\:.*')
    pattern_audio_error = re.compile(r'(AUDIO_ERROR\:|Init Audio Manager|Init File Name|Capture Stream|Release Audio Manager).*')
    pattern_phone_error = re.compile(r'PHONE_ERROR: Broken Socket')
    pattern_script_error = re.compile(r'SCRIPTS_ERROR\:.*')

    jira_issues = {}
    common_issues = {
        'BEEX':{},
        'AUDIO':{},
        'PHONE':{},
        'SCRIPT':{},
    }

    for test_case in report_list:
        error_key = test_case['ErrorsKey']
        if test_case.get("Description", "") != "":
            test_case["Description"] = add_format_for_fail_description(test_case["Description"])

        if error_key and not re.match(pattern_ignore_error, error_key):
            if is_freestyle:
                jira_issues[error_key] = test_case
            else:
                if re.match(pattern_beex_error, error_key) or re.match(pattern_audio_error, error_key) or re.match(pattern_phone_error, error_key) or re.match(pattern_script_error, error_key):
                    #beex error
                    if re.match(pattern_beex_error, error_key):
                        common_key = 'BEEX'
                    #audio error
                    elif re.match(pattern_audio_error, error_key):
                        common_key = 'AUDIO' 
                    #phone error
                    elif re.match(pattern_phone_error, error_key):
                        common_key = 'PHONE'
                    #script error
                    else:
                        common_key = 'SCRIPT'
                    common_issues[common_key][error_key] = test_case
                #soc error and beex error
                if not (re.match(pattern_audio_error, error_key) or re.match(pattern_phone_error, error_key) or re.match(pattern_script_error, error_key)):
                    jira_issues[error_key] = test_case
    return jira_issues, common_issues

def add_jira_info_with_error_key(jira_info_dict, common_issues, bin_version="", cfg="",
                  dut_log_path="", case_fail_rate="", fail_rate="", fail_info_dict={},
                  test_case="", error_key="", fs_fail_rate="", fs_test_case="", local_dut_log_path=""):
    hw_info = jira_info_dict['test_module']

    if "common_issues" not in jira_info_dict.keys():
        jira_info_dict["common_issues"] = {}
    jira_info_dict["common_issues"] = common_issues
    if isinstance(bin_version, str):
        jira_info_dict["bin_version"] = bin_version
    jira_info_dict["fs_fail_rate"] = fs_fail_rate
    jira_info_dict["fs_test_case"] = fs_test_case
    if error_key == '':
        return jira_info_dict
    if "error_key" not in jira_info_dict.keys():
        jira_info_dict["error_key"] = {}
    if error_key not in jira_info_dict["error_key"].keys():
        jira_info_dict["error_key"][error_key] = {}
    jira_info_dict["error_key"][error_key]['test_case'] = fail_info_dict.get("Testcase", "")
    jira_info_dict["error_key"][error_key]['dut_log_path'] = dut_log_path
    jira_info_dict["error_key"][error_key]['case_fail_rate'] = case_fail_rate
    jira_info_dict["error_key"][error_key]["fail_rate"] = fail_rate    
    jira_info_dict["error_key"][error_key]["fail_desc"] = fail_info_dict.get("Description", "")
    jira_info_dict["error_key"][error_key]["check_hardFault"] = fail_info_dict.get("check_hardFault", "")
    jira_info_dict["error_key"][error_key]["check_smp"] = fail_info_dict.get("check_smp", "")
    #jira_info_dict["test_case"][test_case]["error_key"][error_key]["is_tool_issue"] = fail_info_dict["is_tool_issue"]
    if "jira" not in jira_info_dict["error_key"][error_key].keys():
        jira_info_dict["error_key"][error_key]["jira"] = {}
    jira_info_dict["error_key"][error_key]["jira"]["description"] = fail_info_dict.get("TestStep")
    jira_info_dict["error_key"][error_key]["jira"]['test_case'] = fail_info_dict.get("Testcase", "")
    jira_info_dict["error_key"][error_key]["jira"]["hw_info"] = hw_info
    jira_info_dict["error_key"][error_key]["jira"]["cfg"] = cfg

    return jira_info_dict


def find_hardFault_error(fail_desc):
    if fail_desc == '':
        return False
    fail_desc = fail_desc.split('\r\n')
    for line in fail_desc:
        if line.find(r'Hard_Fault_Error') == -1:
            continue
        else:
            print("Find hard fault error log", flush=True)
            return True
    else:
        return False


def get_wave_file(fail_desc):
    if fail_desc == '':
        return None
    wave_file_dir = ''
    s_fail_desc = fail_desc.split('\r\n')
    if len(s_fail_desc) == 1:
        s_fail_desc = fail_desc.split('\n')
    for line in s_fail_desc:
        start1 = line.find('PowerDetect:') + 13
        start2 = line.find('AudioFile:') + 10
        start3 = line.find('FreqDetect:') + 12
        start4 = line.find('EQModeDetect:') + 14
        if start1 != 12 or start2 != 9 or start3 != 11 or start4 != 13:
            end = line.find('.wav') + 4
            if end == 3:
                print("get_wave_file: there is no wave file detected, continue", flush=True)
                continue
            if start1 != 12:
                wave_file_dir += line[start1:end] +'\n'
            elif start2 != 9:
                wave_file_dir += line[start2:end] +'\n'
            elif start3 != 11:
                wave_file_dir += line[start3:end] +'\n'
            else:
                wave_file_dir += line[start4:end] +'\n'
    return wave_file_dir


def build_jenkins_job(wave_files, node_name):
    jenkins_url = 'http://cn4soc-jks.rtkbf.com/'
    build_num = 0
    try:
        #server = Jenkins('http://jenkins.realsil.com.cn/', username, password)
        username = os.getenv("JENKINS_USER")
        password = os.getenv("JENKINS_PWD")
        server = Jenkins(jenkins_url, username, password, requester=CrumbRequester(username=username, password=password, baseurl=jenkins_url))
        job_name = 'Dump~WaveFile'
        parameters={'FILE_LIST': wave_files, 'SLAVE_LABEL': node_name} 
        server.build_job(job_name, parameters)
        while True:
            if len(server[job_name].jenkins.get_queue().get_queue_items_for_job(job_name)) == 0:
                break
        build_num = server[job_name].get_last_buildnumber()
    except Exception as e:
        print("build_jenkins_job: build Dump~WaveFile job fialed ->\n {}, continue".format(e), flush=True)
        build_num = -1
    return build_num


def get_wave_files_from_fail_desc_with_error_key(jira_error_dict, test_type):
    #get all wave files in one jira issue
    wave_files = ''
    fail_desc = jira_error_dict["Description"]
    if get_wave_file(fail_desc):
        wave_files += get_wave_file(fail_desc)
    #build Dump~WaveFile, get build number
    build_num = build_jenkins_job(wave_files, test_info['node_name']) if test_type in ['Nightly', 'Release', 'FreeStyle'] else 1

    return build_num, wave_files


def get_wave_files_from_fail_desc_with_test_case(jira_error_dict, test_type):
    #get all wave files in one jira issue
    build_num = 0
    wave_files = ''
    count = 1
    for fail_action in jira_error_dict.keys():
        if count > 4 and test_type in ['FreeStyle']:
            continue
        for fail_desc in jira_error_dict[fail_action].keys():
            if get_wave_file(fail_desc):
                count += 1
                if count > 4 and test_type in ['FreeStyle']:
                    continue
                wave_files += get_wave_file(fail_desc)
    #build Dump~WaveFile, get build number
    if wave_files.strip('\n').endswith(r'.wav'):
        build_num = build_jenkins_job(wave_files, test_info['node_name']) if test_type in ['Nightly', 'Release', 'FreeStyle'] else 1
    
    return build_num, wave_files


def get_build_url(wave_files, num):
    build_url = ''
    if wave_files == '':
        print('get_build_url: there is no wave files found', flush=True)
        return build_url
    wave_files = wave_files.split('\n')
    for wave_file in wave_files:
        file_name = wave_file.strip('\n').split('\\')[-1]
        if os.path.splitext(file_name)[1] != ".wav":
            print("get_build_url: wave file format error", flush=True)
            return build_url
        full_url = COMMON_URL.format(num) + ARTIF_URL.format(file_name) + '\n'
        build_url += full_url
        print('get_build_url: wave files link is ->\n {}'.format(build_url), flush=True)

    return build_url


def write_jira_info(jira_info_dict, jira_info_file):
    print("Write jira info to {}".format(jira_info_file))
    try:
        with open(jira_info_file, mode='w', newline='', errors='surrogateescape') as fd:
            json.dump(jira_info_dict, fd, indent=4)
    except Exception as e:
        print('Write jira info file error: {}'.format(e), flush=True)
        return False
    return True


def read_error_keys_with_test_case(report_list, is_freestyle=False):
    pattern_ignore_error = re.compile(r'\*IGNORE\*|\*SUCCESS\*.*')
    pattern_bee_error = re.compile(r'BEEX_ERROR\:.*')
    pattern_mplic_error = re.compile(r'MPCLI Download Error')
    pattern_logtool_error = re.compile(r'Log Tool Start Error')
    pattern_bin_error = re.compile(r'Bin Error')
    pattern_input_error = re.compile(r'Input Error')
    pattern_phone_error = re.compile(r'(PHONE_ERROR|Phone Error|Socket Error)\:.*')
    pattern_script_error = re.compile(r'Script Error')

    jira_issues = {}
    fs_fail_rate = ''
    fs_test_case = ''
    common_issues = {
        'BEEX':{},
        'MPCLIDownload':{},
        'LogToolStart':{},
        'BIN':{},
        'Input':{},
        'PHONE':{},
        'SCRIPT':{},
    }

    for test_case in report_list:
        case_name = test_case['Testcase']
        fail_action = test_case['Action']
        error_key = test_case['ErrorsKey']
        fail_desc = test_case['Description']
        result = test_case['Result']
        loop = test_case['CurrentLoop']

        if result == 'Pass':
            if is_freestyle:
                fs_fail_rate = test_case['FailRates']
                fs_test_case = test_case['Testcase']
            continue
        if error_key and not re.match(pattern_ignore_error, error_key):
            if is_freestyle:
                if case_name not in jira_issues.keys():
                    jira_issues[case_name] = {}
                if error_key not in jira_issues[case_name].keys():
                    jira_issues[case_name][error_key]={}
                if fail_action not in jira_issues[case_name][error_key].keys():
                    jira_issues[case_name][error_key][fail_action] = {}
                if fail_desc not in jira_issues[case_name][error_key][fail_action].keys():
                    jira_issues[case_name][error_key][fail_action][fail_desc] = {}
                jira_issues[case_name][error_key][fail_action][fail_desc] = test_case
            else:
                if re.match(pattern_bee_error, error_key) or re.match(pattern_mplic_error, error_key) or re.match(pattern_script_error, error_key) or \
                    re.match(pattern_logtool_error, error_key) or re.match(pattern_bin_error, error_key) or re.match(pattern_input_error, error_key):
                    #beex error
                    if re.match(pattern_bee_error, error_key):
                        common_key = 'BEEX'
                    #MPCLIDownload error
                    elif re.match(pattern_mplic_error, error_key):
                        common_key = 'MPCLIDownload'              
                    #phone error
                    elif re.match(pattern_phone_error, error_key):
                        common_key = 'PHONE'
                    #bin error
                    elif re.match(pattern_bin_error, error_key):
                        common_key = 'BIN'
                    #Input error
                    elif re.match(pattern_input_error, error_key):
                        common_key = 'Input'
                    #LogToolStart error
                    elif re.match(pattern_logtool_error, error_key):
                        common_key = 'LogToolStart'
                    #script error
                    else:
                        common_key = 'SCRIPT'
                    if case_name not in common_issues[common_key].keys():
                        common_issues[common_key][case_name] = {}
                    if error_key not in common_issues[common_key][case_name].keys():
                        common_issues[common_key][case_name][error_key]={}
                    common_issues[common_key][case_name][error_key] = test_case
                #soc error and beex error
                if not (re.match(pattern_mplic_error, error_key) or re.match(pattern_script_error, error_key) or \
                    re.match(pattern_logtool_error, error_key) or re.match(pattern_bin_error, error_key) or re.match(pattern_input_error, error_key)):
                    if case_name not in jira_issues.keys():
                        jira_issues[case_name] = {}
                    if error_key not in jira_issues[case_name].keys():
                        jira_issues[case_name][error_key]={}
                    if fail_action not in jira_issues[case_name][error_key].keys():
                        jira_issues[case_name][error_key][fail_action] = {}
                    if fail_desc not in jira_issues[case_name][error_key][fail_action].keys():
                        jira_issues[case_name][error_key][fail_action][fail_desc] = {}
                    jira_issues[case_name][error_key][fail_action][fail_desc] = test_case
    return jira_issues, common_issues, fs_fail_rate, fs_test_case


def add_jira_info_with_test_case(jira_info_dict, common_issues, bin_version='', cfg="",
                  dut_log_path="", case_fail_rate="", fail_rate="", fail_info_dict={}, 
                  test_case="", error_key="", fs_fail_rate="", fs_test_case="", local_dut_log_path=""):

    
    hw_info = "{}".format(jira_info_dict['test_module'])
    
    if "common_issues" not in jira_info_dict.keys():
        jira_info_dict["common_issues"] = {}
    jira_info_dict["common_issues"] = common_issues
    jira_info_dict["bin_version"] = bin_version
    jira_info_dict["fs_fail_rate"] = fs_fail_rate
    jira_info_dict["fs_test_case"] = fs_test_case
    jira_info_dict["url"] = os.environ['BUILD_URL']
    if test_case == '':
        return jira_info_dict
    if "test_case" not in jira_info_dict.keys():
        jira_info_dict["test_case"] = {}
    if test_case not in jira_info_dict["test_case"].keys():
        jira_info_dict["test_case"][test_case] = {}
    jira_info_dict["test_case"][test_case]['dut_log_path'] = dut_log_path
    jira_info_dict["test_case"][test_case]['case_fail_rate'] = case_fail_rate
    if "error_key" not in jira_info_dict["test_case"][test_case].keys():
        jira_info_dict["test_case"][test_case]["error_key"] = {}
    if error_key not in jira_info_dict["test_case"][test_case]["error_key"].keys():
        jira_info_dict["test_case"][test_case]["error_key"][error_key] = {}
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["fail_rate"] = fail_rate    
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["fail_desc"] = fail_info_dict["issue_desc"]
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["check_hardFault"] = fail_info_dict["check_hardFault"]
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["check_smp"] = fail_info_dict["check_smp"]
    #jira_info_dict["test_case"][test_case]["error_key"][error_key]["is_tool_issue"] = fail_info_dict["is_tool_issue"]
    if "jira" not in jira_info_dict["test_case"][test_case]["error_key"][error_key].keys():
        jira_info_dict["test_case"][test_case]["error_key"][error_key]["jira"] = {}
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["jira"]["description"] = fail_info_dict["description"]
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["jira"]["hw_info"] = hw_info
    jira_info_dict["test_case"][test_case]["error_key"][error_key]["jira"]["cfg"] = cfg

    return jira_info_dict


def get_beex_error_loop(test_case, error_key_dict):
    beex_error = BEEX_ERROR
    beex_loop_dict = {}
    if beex_error in  error_key_dict.keys():
        fail_action_dict = error_key_dict[beex_error]
        for fail_action in fail_action_dict.keys():
            for fail_desc in fail_action_dict[fail_action].keys():
                fail_loop = fail_action_dict[fail_action][fail_desc]['CurrentLoop']
                if fail_loop not in beex_loop_dict.keys():
                    beex_loop_dict[fail_loop] = {}
    return beex_loop_dict


def time_format(time):
    time = time.replace('-', "")
    time = time.replace(' ', "")
    time = time.replace('_', "")
    time = time.replace(':', "")
    time = time.replace('#', "")
    time = time.replace('.', "")
    return time


def jira_time_format(time):
    time = time.split('T')[0]
    time = time.replace('-', "")
    return time


def time_cmp(first_time, second_time):
    return int(first_time) - int(second_time)


def is_beex_loop(fail_loop, fail_time, beex_loop_dict):
    beex_loop = False
    for loop in beex_loop_dict.keys():
        if int(fail_loop) != int(loop):
            continue
        for fail_action in beex_loop_dict[loop].keys():
            beex_fail_time = time_format(beex_loop_dict[loop][fail_action])
            fail_time = time_format(fail_time)
            if time_cmp(fail_time, beex_fail_time) > 0:
                beex_loop = True
    return beex_loop


def get_raw_data(hrs_report_path, label, fail_desc):
    raw_data_path = os.path.join(hrs_report_path, max(os.listdir(hrs_report_path)), r'RawData')
    raw_data = ''
    find_raw_data = False

    if fail_desc == '':
        return raw_data
    
    s_fail_desc = fail_desc.split('\r\n')
    if len(s_fail_desc) == 1:
        s_fail_desc = fail_desc.split('\n')
    for line in s_fail_desc:
        print('debug2:' + line)
        s_res = re.search(r'.*PhoneTime:\s(.*)\s\(Phone(\d+):.*', line)
        if s_res:
            print('debug3')
            phone_time = s_res.group(1).split(r'.')[0]
            phone_num = s_res.group(2)
            break
    else:
        print("Not phone ack error, skip get raw data", flush=True)
        return raw_data
    port = 10086 if int(phone_num) == 1 else 10087
    raw_data_pattern = os.path.join(raw_data_path, r'*{}_RawData*.log'.format(port))
    raw_data_list = glob.glob(raw_data_pattern)
    if len(raw_data_list) != 1:
        print("only 1 {} raw data file should be existed, now {} file(s) existed".format(raw_data_pattern, len(raw_data_list)))
        return raw_data
    raw_data_file = raw_data_list[0]
    with open(raw_data_file) as f:
        lines = f.readlines()
    raw_data += 'RawData(分析手机问题时使用):\n'
    for line in lines:
        res = re.search(r'.*{}.*'.format(phone_time), line)
        if res:
            find_raw_data = True
            raw_data += line
    if not find_raw_data:
        return ''

    return raw_data


def get_full_fail_desc(fail_info_dict, beex_loop_dict, fail_action, error_key, 
                       test_case, jira_issue_list, num, test_type, check_hardFault, 
                       translation_set, hrs_report_path, build_num=0, wave_files=[], 
                       unzip_log_path=''):
    build_url = ''
    wave_file_link = ''
    #exception_log = ''
    #add_desc = True
    for fail_desc in jira_issue_list[test_case][error_key][fail_action].keys():
        fail_info_dict['description'] = jira_issue_list[test_case][error_key][fail_action][fail_desc]["TestStep"]
        fail_info_dict['fail_rate'] = jira_issue_list[test_case][error_key][fail_action][fail_desc].get('FailRates')
        fail_loop = jira_issue_list[test_case][error_key][fail_action][fail_desc].get('CurrentLoop')
        fail_time = jira_issue_list[test_case][error_key][fail_action][fail_desc].get('Time',"")
        if not fail_loop:
            fail_loop = fail_info_dict['fail_rate'].split("/")[0]
        if is_beex_loop(fail_loop, fail_time, beex_loop_dict):
            continue
        issue_desc_common = ISSUE_DESC_COMMON.format(SPLIT_MARK, str(num), SPLIT_MARK, int(float(fail_loop)), fail_action)
        #check exception log
        if translation_set:
            label = translation_set[error_key]["label"] if error_key in translation_set.keys() else ''
        else:
            print("There is no log file to filter exception log", flush=True)
            label = ''
        #check if it is hard fault error
        if check_hardFault:
            result = find_hardFault_error(fail_desc)
            if not result:
                continue
            fail_info_dict['check_hardFault'] = True 
        #check if it is paging timeout error
        #if exception_log and module == 'SMP' and re.search(r'.*(<<Paging_Timeout>>).*', exception_log):
        #    fail_info_dict['check_smp'] = True
        #add_desc = False if exception_log == '44' else True
        #check if it is phone error, add raw data
        raw_data = get_raw_data(hrs_report_path, label, fail_desc)
        #code_desc = r'{code:java}' + fail_desc + exception_log + raw_data + r'{code}' if add_desc else 'Caused by log delay, bypass'
        code_desc = r'{code:java}' + fail_desc + raw_data + r'{code}'
        
        if build_num > 0:
            if test_type in ['CI']:
                wave_file_link = WAVE_FILE.format('\n'.join(wave_files))
            elif test_type not in ['CI'] and num < 4:
                if get_wave_file(fail_desc):
                    build_url = get_build_url(get_wave_file(fail_desc), build_num)
                wave_file_link = WAVE_FILE.format(build_url)
        if build_num == -1:
            wave_file_link = WAVE_FILE.format("Get wave files failed, please get wave files from slave")
            print("Trigger jenkins build failed, please check!", flush=True)
        full_desc = issue_desc_common + wave_file_link + code_desc
        if test_type not in ['CI'] and num > 3:
            num += 1
            continue
        fail_info_dict['issue_desc'] += full_desc
        num += 1

    return fail_info_dict, num


def get_fail_rate(fail_rate, num):
    if fail_rate != '':
        try:
            fail_num = re.search(r'(\d+)/(\d+)', fail_rate).group(1)
            loop = re.search(r'(\d+)/(\d+)', fail_rate).group(2)
        except:
            fail_num = '1'
            loop = '10'
    else:
        fail_num = '1'
        loop = '10'
    real_fail_num = num -1
    if int(fail_num) != real_fail_num:
        if real_fail_num > int(loop):
            real_fail_num = int(loop)
        fail_rate = str(real_fail_num) + '/' + loop
    return fail_rate


def read_report_with_error_key(jira_info_dict, test_type, jira_info_file):
    report_list = read_excel(jira_info_dict.get("report_name", ""))
    is_freestyle = True if test_type in ["FreeStyle"] else False
    jira_issue_list, common_issues = read_error_keys_with_error_key(report_list, is_freestyle=is_freestyle)
    
    common_dir = jira_info_dict['module_name']
    jira_info_dict = add_jira_info_with_error_key(jira_info_dict, common_issues)

    #create jira issue
    for error_key in jira_issue_list.keys():
        print("Read errorkey {}".format(error_key), flush=True)

        if error_key in [BEEX_ERROR] and test_type in ['CI', 'Nightly', 'Release']:
            continue

        jira_issue_list[error_key]['check_hardFault'] = False
        if error_key.lower() in EXCEPTION_LOG and test_type in ['CI', 'Nightly', 'Release']:
            if find_hardFault_error(jira_issue_list[error_key]["Description"]):
                jira_issue_list[error_key]['check_hardFault'] = True
        jira_issue_list[error_key]['check_smp'] = False

        build_num, wave_files = get_wave_files_from_fail_desc_with_error_key(jira_issue_list[error_key], test_type)

        build_url = get_build_url(wave_files, build_num)
        wave_file_link = WAVE_FILE.format(build_url)
        jira_issue_list[error_key]["Description"] = wave_file_link + jira_issue_list[error_key]["Description"]

        test_cases = jira_issue_list[error_key]['Testcase']
        dut_log_path = ""
        local_dut_log_path = ""

        case_fail_rate = jira_issue_list[error_key]['FailRates'].split("\n")[0]
        fail_rate = case_fail_rate[len("Total FailRates: "):] if "Total FailRates: " in case_fail_rate else case_fail_rate

        if test_type in ['CI']:
            cfg = r'{} (cfg_LoopNumber = 3, cfg_maxFailNumber= 3)'.format(fail_rate)
        else:
            cfg = r'{} (cfg_LoopNumber = 500, cfg_maxFailNumber= 10) [{}]'.format(fail_rate, error_key)
        
        print("Add jira info for errorkey {}".format(error_key), flush=True)
        jira_info_dict = add_jira_info_with_error_key(jira_info_dict, common_issues, bin_version="", cfg=cfg,
                                        dut_log_path=dut_log_path, case_fail_rate=case_fail_rate, fail_rate=fail_rate, 
                                        fail_info_dict = jira_issue_list[error_key], error_key=error_key, 
                                        fs_fail_rate=jira_issue_list[error_key]['FailRates'], fs_test_case=test_cases, local_dut_log_path=local_dut_log_path)

        if not write_jira_info(jira_info_dict, jira_info_file):
            print("Write jira info for {} failed".format(error_key), flush=True)
            continue
    else:
        print("Add common issues for module {}".format(test_module), flush=True)
        jira_info_dict["common_issues"] = common_issues

        if not write_jira_info(jira_info_dict, jira_info_file):
            print("Write jira info for common issues failed", flush=True)


def read_report_with_test_case(jira_info_dict, test_type, jira_info_file):
    report_file = jira_info_dict.get("report_name", "")
    if not os.path.exists(report_file):
        print("Cannot find specified file {}". format(report_file), flush=True)
        return

    jira_issue_list = list()
    #read report

    report_list = read_excel(report_file)

    is_freestyle = True if test_type in ["FreeStyle"] else False
    jira_issue_list, common_issues, fs_fail_rate, fs_test_case = read_error_keys_with_test_case(report_list, is_freestyle=is_freestyle)

    if len(jira_issue_list.keys()) > 1 and test_type in ['FreeStyle']:
        print("only 1 test case should be verified, now {} cases existed".format(len(jira_issue_list.keys())))
        return False
    
    common_dir = jira_info_dict['module_name']
    hrs_report_path = os.path.dirname(jira_info_dict["report_name"])
    base_dir = os.path.join(os.getcwd(), r'unzip')
    url = os.environ['BUILD_URL']

    jira_info_dict = add_jira_info_with_test_case(jira_info_dict, common_issues, fs_test_case=fs_test_case, fs_fail_rate=fs_fail_rate)

    for test_case in jira_issue_list.keys():
        if not test_case:
            continue
        print("Read test case {}".format(test_case), flush=True)
        beex_loop_dict = {}
        if test_type in ['Nightly', 'Release']:
            beex_loop_dict = get_beex_error_loop(test_case, jira_issue_list[test_case])

        local_dut_log_path = os.path.join(os.getcwd(),r'log',common_dir)
        unzip_log_path = ''
        if os.path.exists(base_dir):
            shutil.rmtree(base_dir)
        os.makedirs(base_dir,exist_ok=True)
        #if test_module not in MODULE_GROUP_MAP['AutoTest']:
        #    unzip_log_path =  unzip_log_file(local_log_path, base_dir, test_case, test_type)

        for error_key in jira_issue_list[test_case].keys():
            num = 1
            check_hardFault = 0
            fail_info_dict = dict()
            fail_info_dict['issue_desc'] = ''
            fail_info_dict['check_hardFault'] = False
            fail_info_dict['check_smp'] = False

            if error_key in [BEEX_ERROR] and test_type in ['CI', 'Nightly', 'Release']:
                continue
            #get fail description
            build_num, wave_files = get_wave_files_from_fail_desc_with_test_case(jira_issue_list[test_case][error_key], test_type)
            if test_type in ['CI'] and wave_files != '':
                wave_files = wave_files.split('\n')
                wave_files = list(filter(lambda w: w, wave_files))
                tool_pattern = "HRS"
                if not re.search(tool_pattern, wave_files[0]):
                    tool_pattern = "HRL"

                wave_files = list(map(lambda w: jira_info_dict["log_path"] + quote(w[os.path.abspath(w).index(tool_pattern):].replace("\\", r"/").replace("//", r"/"), "utf-8"), wave_files))

            for fail_action in jira_issue_list[test_case][error_key].keys():
                fail_info_dict, num = get_full_fail_desc(fail_info_dict, beex_loop_dict, fail_action, error_key, 
                                                         test_case, jira_issue_list, num, test_type, check_hardFault, 
                                                         "", hrs_report_path, build_num=build_num, 
                                                         wave_files=wave_files, unzip_log_path=unzip_log_path)
                if num > 4 and test_type not in ['CI']:
                    break
            
            case_fail_rate = fail_info_dict['fail_rate']
            fail_rate = get_fail_rate(fail_info_dict['fail_rate'], num)
            if fail_info_dict['issue_desc'] == '':
                print("Empty fail description, pass.", flush=True)
                continue

            if test_type in ['CI']:
                cfg = r'{} (cfg_LoopNumber = 3, cfg_maxFailNumber= 3)'.format(fail_rate)
            else:
                cfg = r'{} (cfg_LoopNumber = 500, cfg_maxFailNumber= 10) [{}]'.format(fail_rate, error_key)
            
            print("Add jira info for test case {} {}".format(test_case, error_key), flush=True)
            jira_info_dict = add_jira_info_with_test_case(jira_info_dict, common_issues, cfg=cfg,
                                            dut_log_path="", case_fail_rate=case_fail_rate, fail_rate=fail_rate, 
                                            fail_info_dict=fail_info_dict, test_case=test_case, error_key=error_key, 
                                            fs_fail_rate=fs_fail_rate, fs_test_case=fs_test_case, local_dut_log_path=local_dut_log_path)
            
            if not write_jira_info(jira_info_dict, jira_info_file):
                print("Write jira info for {} {} failed".format(test_case, error_key), flush=True)
                continue
    else:
        print("Add common issues for module {}".format(test_module), flush=True)
        jira_info_dict["common_issues"] = common_issues

        if not write_jira_info(jira_info_dict, jira_info_file):
            print("Write jira info for common issues failed", flush=True)


def read_jira_info(jira_info_file):
    is_empty = False
    jira_info = dict()
    try:
        with open(jira_info_file) as f:
            jira_info = json.load(f)
    except Exception as e:
        print("Failed to open jira info file {}: {}".format(jira_info_file, e))
        is_empty = True
    if not jira_info:
        print("read_jira_info: {} is empty, skip".format(jira_info_file), flush=True)
        is_empty = True
    
    return jira_info, is_empty


def is_test_exists(jira_info):
    if  "error_key" not in jira_info.keys() and "test_case" not in jira_info.keys() and test_module not in ["Platform"]:
        print("{} only has common issues, no need to create jira".format(jira_info.get('test_module', '')), flush=True)
        return False
    else:
        print("There are error keys, read error keys.", flush=True)
        return True


def is_issue_exist(jira, test_module, summary, issue_owner = '', hardFault=False, jira_id = None):
    result = True
    issue = None
    if test_module in test_module_map.keys():
        if jira_id:
            contained_task = jira_id
        else:
            contained_task = test_module_map[test_module].get(test_type).get("issue_key")
        sub_task = jira.issue(contained_task)
        for item in sub_task.fields.issuelinks:
            if 'outwardIssue' not in item.raw.keys():
                continue
            if result == False:
                break
            if item.outwardIssue.fields.summary == summary:
                retry_index = 0
                while retry_index < 3:
                    try:
                        issue = jira.issue(item.outwardIssue.key)
                        if (issue.fields.reporter.name == JIRA_ISSUE_REPORTER) and issue.fields.status.name not in ['Closed', "Resolved"]:
                            result = False
                            print('is_issue_exist: Issue {} already exists, add comment for this issue'.format(item.outwardIssue.key), flush=True)
                        break
                    except Exception as e:
                        print('is_issue_exist: error {}'.format(e))
                        print("retry ....", flush=True)
                        retry_index += 1
                        continue
                else:
                    print("Retry 3 times for getting issue {} failed, please check!".format(item.outwardIssue.key), flush=True)
    else:
        print("is_issue_exist: there is no issue key for {}".format(test_module), flush=True)
        return result, issue

    return result, issue


def get_issue_dict(project, summary, test_module, description, assignee, hw_info, cfg, test_case, test_xml, test_type):
    if test_type in ['CI', 'FreeStyle']:
        labels = JIRA_ISSUE_LABELS + [r'CI']
    elif test_type in ['Release']:
        labels = JIRA_ISSUE_LABELS.copy()
        labels.remove(test_module_map[test_module]["cut_label"][0])
        labels.append(r'Release')
    else:
        labels = JIRA_ISSUE_LABELS + [r'Nightly']
    issue_dict = test_module_map[test_module]["issue_dict"]
    issue_dict["project"] = {'key': project}
    if len(summary) > 255:
        issue_dict["summary"] = summary[:254]
    else:
        issue_dict["summary"] = summary
    issue_dict["summary"] = summary
    issue_dict["description"] = description
    issue_dict["labels"] = labels
    issue_dict["assignee"] = {'name': assignee}
    issue_dict["customfield_10609"] = hw_info
    if len(cfg) > 255:
        issue_dict["customfield_10007"] = cfg[:250]
    else:
        issue_dict["customfield_10007"] = cfg
    if test_xml:
        issue_dict["customfield_10008"] = r'[{}] [{}] {}'.format(test_module, test_xml, test_case)
    else:
        issue_dict["customfield_10008"] = r'[{}] {}'.format(test_module, test_case)

    return issue_dict


def create_jira_issue(jira, issue_dict):
    issue = None
    try:
        issue = jira.create_issue(fields=issue_dict)
    except Exception as e:
        jira.kill_session()
        sys.exit('create_jira_issue: error {}'.format(e))
    
    return issue


def get_default_owner(test_module):
    if test_module in ["HoneyGUI_Monkey_Test"]:
        issue_owner = HoneyGUI_OWNER
    return issue_owner
    

def create_jira_for_CI(jira_info_file, test_type, output_detail=None):
    jira_info, is_empty = read_jira_info(jira_info_file)
    if is_empty or not is_test_exists(jira_info):
        print("Jira info file is empty", flush=True)
        return
    if "error_key" in jira_info.keys():
        create_jira_for_CI_with_error_key(jira_info, test_type, output_detail)
    else:
        create_jira_for_CI_with_test_case(jira_info, test_type, output_detail)


def create_jira_for_CI_with_error_key(jira_info, test_type, output_detail=None):
    reported_jira_detail = {}
    translation_set = {}

    if not output_detail:
        print("No output file setted", flush=True)
    if output_detail and jira_info:
        with open(output_detail, 'a',encoding='utf-8') as fd:
            for c in jira_info.get("error_key", {}):
                fd.write("[{}]\t[{}]\t[{}]\t{}\t{}\n".format(time.strftime("%y%m%d_%H%M%S", time.localtime()), "HoneyGUI", HoneyGUI_branch, c, translation_set.get(c, {}).get("label", "")))
    else:
        print("Nothing to report", flush=True)
    
    jira = connect_jira()

    for error_key in jira_info.get("error_key", {}).keys():
        count = 1
        hardFault = False
        if count > 1:
            break
        count += 1
        issue_owner = r'meeyi_zhao'
        module_info = jira_info['test_module']

        print("No error_key {} in translation_set".format(error_key), flush=True)
        summary = r'[CI][{}][{}][{}]{}'.format("HoneyGUI", HoneyGUI_branch, module_info, error_key)
        
        if jira_info["error_key"][error_key]['check_hardFault']:
            summary = r'[CI][{}][{}]Hard Fault Error!'.format("HoneyGUI", HoneyGUI_branch)
            issue_owner = jira_info["error_key"][error_key]["RD"]
            print("This is a hard fault error, assign this issue to {}".format(issue_owner), flush=True)
            hardFault = True
        
        create_issue = True
        create_issue, issue = is_issue_exist(jira, test_module, summary, issue_owner)

        # Create issue if not found
        test_case = jira_info["error_key"][error_key]["jira"]['test_case'].split("\n")[0]
        if create_issue == True:
            issue_dict = get_issue_dict(PROJECT, summary, jira_info['test_module'],
                                        jira_info["error_key"][error_key]["jira"]['description'], 
                                        issue_owner, jira_info["error_key"][error_key]["jira"]['hw_info'], 
                                        jira_info["error_key"][error_key]["jira"]['cfg'], test_case, 
                                        jira_info["xml_name"], test_type)
            try:
                if error_key in translation_set.keys():
                    error_key_label = translation_set.get(error_key, {}).get("label", "")
                    if isinstance(error_key_label,list):
                        for l in error_key_label:
                            issue_dict['labels'].append(l)
                    else:
                        issue_dict['labels'].append(error_key_label)
                issue = create_jira_issue(jira, issue_dict)
                print('create_jira: Create issue {}'.format(issue.key), flush=True)
                jira.add_comment(issue, FREE_STYLE_COMMENT.format(test_module_map[test_module]["freestyle_link"], test_case))
                #jira.issue_link_types() get issue link types
                jira.create_issue_link('Contains(WBSGantt)', test_module_map[test_module].get(test_type).get("issue_key"), issue.key) #this issue is contained in BBLITERD-80
            except Exception as e:
                print('create_jira: error {}'.format(e))
        else:
            try:
                labels = issue.fields.labels
                if error_key in translation_set.keys():
                    error_key_label = translation_set.get(error_key, {}).get("label", "")
                    if isinstance(error_key_label,list):
                        for l in error_key_label:
                            labels.append(l)
                    else:
                        labels.append(error_key_label)
            except Exception as e:
                print('update_jira: error {}'.format(e))
        event_owner = os.environ.get("GERRIT_CHANGE_OWNER_EMAIL", None)
        comment_detail = ISSUE_COMMENT.format(jira_info['test_module'], jira_info['node_name'],
                                                HoneyGUI_branch, HoneyGUI_version,
                                                os.environ['BUILD_URL'], jira_info['log_path'], 
                                                test_case, 
                                                jira_info["error_key"][error_key]['fail_rate'], 
                                                jira_info["error_key"][error_key]['fail_desc'])
        comment_detail = "Dear [~{}] and [~{}]\n".format(event_owner[0:event_owner.index(r'@')], translation_set.get(error_key, {}).get("rdName", get_default_owner(jira_info['test_module']))) + comment_detail
        jira.add_comment(issue, comment_detail)
        reported_jira_detail[test_case] = reported_jira_detail.get(test_case,[])
        reported_jira_detail[test_case].append(issue.key)
    
    if reported_jira_detail:
        summary_for_rd = ""
        for tc in reported_jira_detail:
            summary_for_rd  = summary_for_rd if summary_for_rd else summary_for_rd + r"\n"
            summary_for_rd +="[{}] {}".format(jira_info['test_module'], tc) + " fail, Please refer to jira: " + " ".join(list(map(lambda o: r'https://jira.realtek.com/browse/{}'.format(o), reported_jira_detail[tc]))) + r'\n'
        output_summary_file = os.path.join(os.path.dirname(output_detail), "summary.txt")
        with open(output_summary_file, 'a',encoding='utf-8',newline='') as fd:
            fd.write(summary_for_rd)


def create_jira_for_CI_with_test_case(jira_info, test_type, output_detail=None):
    if not os.path.exists(jira_info_file):
        print("create_jira_for_CI: {} doesn't exists".format(jira_info_file), flush= True)
        return
    
    jira_info = {}
    reported_jira_detail = {}
    translation_set = {}

    jira_info, is_empty = read_jira_info(jira_info_file)
    if is_empty or not is_test_exists(jira_info):
        print("Jira info file is empty", flush=True)
        return
    
    if not output_detail:
        print("No output file setted", flush=True)
    
    if output_detail and jira_info:
        with open(output_detail, 'a',encoding='utf-8') as fd:
            for c in jira_info.get("test_case", []):
                fd.write("[{}]\t[{}]\t[{}]\t{}\t{}\t{}\n".format(time.strftime("%y%m%d_%H%M%S", time.localtime()), HoneyGUI_branch, HoneyGUI_version, c, list(jira_info["test_case"][c]["error_key"])[0], translation_set.get(list(jira_info["test_case"][c]["error_key"])[0], {}).get("label", "")))
    else:
        print("Nothing to report", flush=True)
    
    jira = connect_jira()

    for test_case in jira_info.get("test_case", {}).keys():
        print("test_case:",test_case)
        count = 1
        for error_key in jira_info["test_case"][test_case]["error_key"].keys():
            hardFault = False
            if count > 1:
                break
            count += 1
            issue_owner = r'meeyi_zhao'
            module_info = jira_info['test_module']
            if error_key in translation_set.keys():
                summary = r'[CI][{}][{}][{}]{}'.format("HoneyGUI", HoneyGUI_branch, module_info, translation_set.get(error_key, {}).get("label", ""))
            else:
                print("No error_key {} in translation_set".format(error_key), flush=True)
                summary = r'[CI][{}][{}][{}]{}'.format("HoneyGUI", HoneyGUI_branch, module_info, error_key)
            print("summary",summary)
            
            create_issue = True
            create_issue, issue = is_issue_exist(jira, test_module, summary, issue_owner)

            if create_issue == True:
                issue_dict = get_issue_dict(PROJECT, summary, jira_info['test_module'],
                                            jira_info["test_case"][test_case]["error_key"][error_key]["jira"]['description'], 
                                            issue_owner, jira_info["test_case"][test_case]["error_key"][error_key]["jira"]['hw_info'], 
                                            jira_info["test_case"][test_case]["error_key"][error_key]["jira"]['cfg'], test_case, 
                                            jira_info["xml_name"], test_type)
                try:
                    if error_key in translation_set.keys():
                        error_key_label = translation_set.get(error_key, {}).get("label", "")
                        if isinstance(error_key_label,list):
                            for l in error_key_label:
                                issue_dict['labels'].append(l)
                        else:
                            issue_dict['labels'].append(error_key_label)
                    issue = create_jira_issue(jira, issue_dict)
                    print('create_jira: Create issue {}'.format(issue.key), flush=True)
                    jira.add_comment(issue, FREE_STYLE_COMMENT.format(test_module_map[test_module]["freestyle_link"], test_case))
                    #jira.issue_link_types() get issue link types
                    jira.create_issue_link('Contains(WBSGantt)', test_module_map[test_module].get(test_type).get("issue_key"), issue.key) #this issue is contained in BBLITERD-80
                except Exception as e:
                    print('create_jira: error {}'.format(e))
            else:
                try:
                    labels = issue.fields.labels
                    if error_key in translation_set.keys():
                        error_key_label = translation_set.get(error_key, {}).get("label", "")
                        if isinstance(error_key_label,list):
                            for l in error_key_label:
                                labels.append(l)
                        else:
                            labels.append(error_key_label)
                except Exception as e:
                    print('update_jira: error {}'.format(e))
            event_owner = os.environ.get("GERRIT_CHANGE_OWNER_EMAIL", None)
            comment_detail = ISSUE_COMMENT.format(jira_info['test_module'], jira_info['node_name'],
                                                HoneyGUI_branch, HoneyGUI_version,
                                                os.environ['BUILD_URL'], jira_info['log_path'], 
                                                test_case, 
                                                jira_info["test_case"][test_case]["error_key"][error_key]['fail_rate'], 
                                                jira_info["test_case"][test_case]["error_key"][error_key]['fail_desc'])
            comment_detail = "Dear [~{}] and [~{}]\n".format(event_owner[0:event_owner.index(r'@')], translation_set.get(error_key, {}).get("rdName", get_default_owner(jira_info['test_module']))) + comment_detail
            jira.add_comment(issue, comment_detail)
            reported_jira_detail[test_case] = reported_jira_detail.get(test_case,[])
            reported_jira_detail[test_case].append(issue.key)
        break

    if reported_jira_detail:
        summary_for_rd = ""
        for tc in reported_jira_detail:
            summary_for_rd  = summary_for_rd if summary_for_rd else summary_for_rd + r"\n"
            summary_for_rd +="[{}] {}".format(jira_info['test_module'], tc) + " fail, Please refer to jira: " + " ".join(list(map(lambda o: r'https://jira.realtek.com/browse/{}'.format(o), reported_jira_detail[tc]))) + r'\n'
        output_summary_file = os.path.join(os.path.dirname(output_detail), "summary.txt")
        with open(output_summary_file, 'a',encoding='utf-8',newline='') as fd:
            fd.write(summary_for_rd)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='read test report and create jira script')
    parser.add_argument('--dir', help='Test report directory')
    parser.add_argument('--chipType', choices=['rtl87x2g'], help='Chip Type')
    parser.add_argument('--testModule', help='Test module')
    parser.add_argument('--archivePath', help='Archive path')
    parser.add_argument('--output', help='Output file path')
    parser.add_argument('--jira', help='FreeStyle jira ID')
    parser.add_argument('--commentFile', help='jira comment record file for freestyle')
    parser.add_argument('--testType', choices=['CI', 'Nightly', 'Release', 'FreeStyle'], default='Nightly', help='Test type')
    parser.add_argument('--step', choices=['Create', 'Read', 'Write', 'Dump', 'CloseJira', 'WriteComment'], default='Read', help='Step')
    parser.add_argument('--uploadBin', action = 'store_true', help='use the bin user uploaded, only used by freestyle')
    parser.add_argument('--jiraID', help='jira id')
    parser.add_argument('--index', help='sequence index')
    parser.add_argument('--projectList', nargs = '+', help='project list')
    parser.add_argument('--toolTest', default='false', help='tooltest jira will not assign to rd')

    arg_dict = parser.parse_args()
    report_dir = arg_dict.dir
    chip_type = arg_dict.chipType
    test_module = arg_dict.testModule
    archive_path = arg_dict.archivePath

    comment_file = arg_dict.commentFile
    jira_id = arg_dict.jira
    test_type = arg_dict.testType
    step = arg_dict.step
    upload_bin = arg_dict.uploadBin
    index = arg_dict.index or 0
    index = int(index)
    script_path = os.path.join(os.getenv("WORKSPACE"), r"script")
    is_tool_test = True if arg_dict.toolTest == 'true' else False
    test_module_map, jira_info_map = create_jira_config._init(chip_type)

    HoneyGUI_branch = os.environ['HoneyGUI_BranchName']
    HoneyGUI_version = os.environ['HoneyGUI_CommitID']

    PROJECT = jira_info_map.get(chip_type).get("project")
    JIRA_ISSUE_LABELS = test_module_map.get(test_module, {}).get("jira_issue_labels", []) + test_module_map.get(test_module, {}).get("cut_label", [])

    print("Step is {}".format(step), flush=True)
    if step == 'Read':
        from py7zr import unpack_7zarchive
        shutil.register_unpack_format('7zip', ['.7z'], unpack_7zarchive, description='7zip archive')

        jira_info_file_list = glob.glob(os.path.join(report_dir, r'jira_info*.json'))
        print("Read report: {}".format(jira_info_file_list))
        for jira_info_file in jira_info_file_list:
            test_info = read_test_info(jira_info_file)
            try:
                read_report(test_info, test_type, jira_info_file)
            except Exception as e:
                print("Read report failed for {}: {}".format(test_info.get('test_module', ""), e), flush=True)
    elif step == 'Create':
        import requests
        from jira import JIRA
        from atlassian import Confluence

        jira_info_match_pattern = r'jira_info~{}*.json'.format(test_module)
        jira_info_file_list = glob.glob(os.path.join(report_dir, jira_info_match_pattern))
        for jira_info_file in jira_info_file_list:
            if test_type in ["CI"]:
                output_detail = arg_dict.output
                create_jira_for_CI(jira_info_file, test_type, output_detail)
            else:
                print("Test type didn't match CI/Nightly/Release/FreeStyle, skip.", flush=True)
