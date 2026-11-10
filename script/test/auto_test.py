#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import shutil
import os
import subprocess
import log
import json
import sys

class AutoTest():
    def __init__(self, sequence_index, test_type, 
                module,  **kwargs) -> None:
        self.sequence_index = sequence_index
        self.test_type = test_type
        self.module = module

        self.is_dryrun = kwargs.get("is_dryrun", False)
        self.skip_download_bin = kwargs.get("skip_download_bin", False)
        self.triggered_by = kwargs.get("triggered_by", "")
        self.triggered_date = kwargs.get("triggered_date", "")
        self.is_smoke = kwargs.get("is_smoke", False)


    def run_test(self):
        log_path = log.get_log_dir(module = self.module)

        cmd = ['python', f"{os.getcwd()}\\script\\IntegrationTester"]
        cmd.extend(["--testType", self.test_type])
        if self.module in ["HoneyGUI_CMD"]:
            cmd.extend(["--module", "HoneyGUI"])
        else:
            cmd.extend(["--module", self.module])
        if self.is_dryrun:
            cmd.extend(["--dryrun"])

        cmd.extend(["--binDir", os.path.join(os.getcwd(), "SRC", self.module, "bin")])
        cmd.extend(["--logDir", log_path])
        cmd.extend(["--reportDir", os.path.join(os.getcwd(), r"SRC\report")])

        integrateion_config_data = dict()
        integrateion_config_json = os.path.join(os.getcwd(), "script", r"SDKPlatformConfig\config_file.json")
        with open(integrateion_config_json) as fd:
            integrateion_config_data = json.load(fd)

        sequence_dict = integrateion_config_data.get("sequence_config", {}).get(self.module, {})
        additional_parameters = sequence_dict.get(self.test_type + "-Addi-Params", list())

        if additional_parameters:
            cmd.extend(additional_parameters)

        test_cases = os.environ.get("TestCases")
        if test_cases:
            test_cases = test_cases.split("\n")
            test_cases = map(lambda t:t.strip("[").strip("]"), test_cases)
            test_cases = list(filter(lambda o:o, test_cases))
        if os.environ.get("Loop_Number"):
            cmd.extend(["--loop", os.environ.get("Loop_Number")])
        if test_cases:
            cmd.extend(["--tcList"] + test_cases)

        cmd_list = list()
        if test_cases:
            cmd_list = [cmd]
        else:
            sequence_dict = integrateion_config_data.get("sequence_config", {}).get(self.module, {})
            sequence_dict = sequence_dict.get(self.test_type+"-Submodules", {}) or sequence_dict.get(self.test_type, {})
            if not sequence_dict:
                sys.exit("Can not find the sequence")

            if isinstance(sequence_dict, dict):
                for id in sequence_dict:
                    cmd_list.append(cmd + ["--xmlPath", sequence_dict[id], "--reportPrefix", id])
            else:
                cmd_list = [cmd + ["--xmlPath", sequence_dict]]

        print("cmd_list",cmd_list)
        result = True

        for t_cmd in cmd_list:
            try:
                print(" ".join(t_cmd), flush=True)
                subprocess.check_call(t_cmd, universal_newlines=True, stderr=subprocess.STDOUT)
            except subprocess.CalledProcessError as e:
                print("Call {} fail".format(t_cmd))
                result = False
        if not result:
            sys.exit("call {} test fail".format(self.module))

