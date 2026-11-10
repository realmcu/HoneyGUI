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
import queue
import threading
from slave_property import *
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from peripheral.bee1_controller import Bee1Controller
from base.tool_base import ToolBase

west_home = os.environ.get("WestHome")
application_path = os.path.join(west_home, r"applications")
current_path = os.getcwd()

class ZephyrAutoTest():
    def __init__(self, project, branch, test_type, chip_type, 
                module, part_number, **kwargs) -> None:
        self.project = project
        self.branch = branch
        self.test_type = test_type
        self.chip_type = chip_type
        self.module = module
        self.part_number = part_number
        self.result_queue = None

        self.is_dryrun = kwargs.get("is_dryrun", False)
        self.skip_download_bin = kwargs.get("skip_download_bin", False)
        self.triggered_by = kwargs.get("triggered_by", "")
        self.triggered_date = kwargs.get("triggered_date", "")
        self.is_smoke = kwargs.get("is_smoke", False)

    def run_mpcli_mp_loader_cus(self, wait_timeout):
        print("wait the mp loader event")
        try:
            res = self.result_queue.get(block=True, timeout=wait_timeout)
            print("Got {}, no need to restart soc".format(res))
        except queue.Empty as e:
            print("No expected message got, try to reset soc")
            bee1_controller = Bee1Controller(self.project, self.branch, self.chip_type, self.role, self.module, self.part_number)
            bee1_controller.test_cut_restart(interval=0)

    def run_test(self):
        os.chdir(application_path)
        mpcli = slave_property_parse(self.project, self.branch, self.chip_type, 'MPCLI', 'role', 'DUT1', part_number=self.part_number)
        download_port = mpcli['COM']
        self.result_queue = queue.Queue(10000) 
        thread_download = threading.Thread(target=self.run_mpcli_mp_loader_cus, kwargs={"wait_timeout": 5})
        thread_download.start()

        tool_base = ToolBase(cmd_line=cmd, exit_once_failed=False,timeout=self.timeout,  wait_event= {"message": "run into mp loader", "callback": self.run_mpcli_catch_mp_call_back})
        download_bin_res = tool_base.tool_run()
        if not download_bin_res:
            return False
        self.result_queue.queue.clear()
        subprocess.check_call([r'scripts/flash-win32-app.bat', download_port], universal_newlines=True)
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

