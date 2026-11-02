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

    def run_mpcli_catch_mp_call_back(self, message):
        print(message)
        self.result_queue.put(message)

    def run_mpcli_mp_loader_cus(self, wait_timeout):
        print("wait the mp loader event")
        try:
            res = self.result_queue.get(block=True, timeout=wait_timeout)
            print("Got {}, no need to restart soc".format(res))
        except queue.Empty as e:
            print("No expected message got, try to reset soc")
            bee1_controller = Bee1Controller(self.project, self.branch, self.chip_type, "DUT1", self.module, self.part_number)
            bee1_controller.test_cut_restart(interval=0)

    def run_test(self):
        mpcli = slave_property_parse(self.project, self.branch, self.chip_type, 'MPCLI', 'role', 'DUT1', part_number=self.part_number)
        download_port = mpcli['COM']
        origin_path = os.getcwd()
        os.environ["NOPAUSE"] = "1"
        os.chdir(os.path.join(os.environ["WORKSPACE"], "applications"))
        try:
            self.result_queue = queue.Queue(10000)
            thread_download = threading.Thread(target=self.run_mpcli_mp_loader_cus, kwargs={"wait_timeout": 5})
            thread_download.start()
            cmd = [r'.\scripts\flash-win32-app.bat', download_port]
            tool_base = ToolBase(cmd_line=cmd, exit_once_failed=False,timeout=180,  wait_event= {"message": "run into mp loader", "callback": self.run_mpcli_catch_mp_call_back})
            download_bin_res = tool_base.tool_run()
        finally:
            os.chdir(origin_path)
        if not download_bin_res:
            return False
        self.result_queue.queue.clear()
        log_path = log.get_log_dir(module = self.module)
 
        integrationTester_config = slave_property_parse(self.project, self.branch, self.chip_type, 'IntegrationTester', 'role', 'ALL', part_number=self.part_number)
        cmd = ['python', integrationTester_config["path"]]
        cmd.extend(["--testType", self.test_type])
        if self.module in ["HoneyGUI_Zephyr"]:
            cmd.extend(["--module", "HoneyGUI"])
        else:
            cmd.extend(["--module", self.module])
        cmd.extend(["--IC", self.chip_type])
        cmd.extend(["--bankType", "BANK0"])
        cmd.extend(["--logDir", log_path])
        cmd.extend(["--xmlPath", integrationTester_config["sequence"]])
        cmd.extend(["--devConfigFile", integrationTester_config["config"]])

        try:
            print(" ".join(cmd), flush=True)
            subprocess.check_call(cmd, universal_newlines=True, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            print("Call {} fail".format(cmd))
            result = False
        if not result:
            sys.exit("call {} test fail".format(self.module))

