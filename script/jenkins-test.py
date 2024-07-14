#!/usr/bin/env python3

import os
import sys
import shutil
import argparse
import importlib
from jenkins_test_target_config import *
from ftp.ftp_helper import *
import log
import json
from report.hrs_report import *


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='bb2-sdk-v1 test script')
    parser.add_argument('testType', choices=['CI', 'Nightly', 'Release', 'FreeStyle'], help='set the test tpye')
    parser.add_argument('sequenceIndex', help='sequence group to test')
    parser.add_argument('--module', help='Set Module to test')
    parser.add_argument('--dryrun', action = 'store_true', help='is dry run')
    parser.add_argument('--step', default='TEST', choices=['DUMPLOG', 'TEST', 'ZIPLOG'])
    parser.add_argument('--triggeredBy', default='gerrit', choices=['gerrit', 'USER', 'TIMER', 'SCM','HRL_Tool_CI','HRL_Tool_Release', "HRS_Tool_CI", "HRS_Tool_Release"])
    parser.add_argument('--triggeredDate', default='', help='time the job triggerd') #yyyy/mm/dd format

    arg_dict = parser.parse_args()
    test_type = arg_dict.testType
    sequence_index = arg_dict.sequenceIndex
    module = arg_dict.module
    triggered_by = arg_dict.triggeredBy

    if module not in ALL_SUPPORTED_MODULE:
        sys.exit("invalid module {}, supported: {}".format(module, ALL_SUPPORTED_MODULE))

    is_dryrun = arg_dict.dryrun
    step = arg_dict.step
    triggered_date = arg_dict.triggeredDate


    if step == 'DUMPLOG':
        log_path = log.get_log_dir(module = module)
        copy_report("HoneyGUI", module, sequence_index, test_type, is_dryrun, "scons",log_path)
        log.copy_log_to_ftp(os.getcwd(), log_path, module)
    else:
        sys.path.append(os.path.join(os.path.dirname(__file__), "test"))
        test_class_module_name = importlib.import_module(MODULE_APP_MAP[module]['test_class_module'])
        test_class_name = "".join(list(map(lambda x: x.capitalize(), MODULE_APP_MAP[module]['test_class_module'].split("_"))))
        test_class = getattr(test_class_module_name, test_class_name)(sequence_index = sequence_index,
                                                                    test_type = test_type, 
                                                                    module = module, is_dryrun = is_dryrun,
                                                                    triggered_by = triggered_by, triggered_date = triggered_date)
        test_class.run_test()
