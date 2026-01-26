#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import sys
import shutil
import glob
import re
from urllib.parse import quote
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from base.parse_json import ParseJson


def read_test_info():
    test_result_info = dict()
    local_report_folder = os.path.join(os.getcwd(), r"SRC\report")
    for f in glob.glob(os.path.join(local_report_folder, r'jira_info~*.json')):
        print(f"get_jira_info: {f}")
        res = re.search(r"jira_info~(.*).json", os.path.basename(f))
        module = res.group(1)
        if not test_result_info.get(module):
            test_result_info[module] = dict()
        test_result_info[module].update(ParseJson.read(f))

    return test_result_info


def generate_html_report(test_result_info):
    import dominate
    from dominate.tags import a, li, h1, h2, h3, b
    test_result = os.getenv("Test_Result", "FAILURE")
    doc = dominate.document()
    with doc:
        h1('Test Report')
        with h2():
            b("Test Result: ")
            if test_result != "SUCCESS":
                b(test_result, style="color:red")
            else:
                b(test_result)
        for module in test_result_info.keys():
            with h3(module):
                if test_result_info[module]["result"] != "true":
                    b("FAILURE", style="color:red")
                else:
                    b("SUCCESS")
                b(test_result_info[module]["node_name"])
                for key, func in [("bin_path", "Test bin: "), ("log_path", "Log: ")]:
                    with li():
                        b(func)
                        a(test_result_info[module][key], href=test_result_info[module][key], style="color:blue;text-decoration:none")
    
    report_path = os.path.join(os.getcwd(), r"SRC/report")
    os.makedirs(report_path, exist_ok=True)
    with open(os.path.join(report_path, "index.html"), "w", encoding='utf-8') as f:
        f.write(str(doc))


if __name__ == '__main__':
    test_result_info = read_test_info()
    generate_html_report(test_result_info)
