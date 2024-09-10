import os
import sys
import time
import threading
import queue
import subprocess
import serial.tools.list_ports
from filelock import FileLock
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import log
from jenkins_test_target_config import *
from slave_property import *
from base.tool_base import ToolBase
from tool.honey_robot_light import HoneyRobotLight
from common_func import *
from error_record import *
from report.hrs_report import *
from tool.tool_func import test_tool_communication


class HoneyguiMonkeyTest():
    def __init__(self, project, branch, sequence_index, test_type,
                module, **kwargs):
        self.project = project
        self.branch = branch
        self.sequence_index = sequence_index
        self.test_type = test_type
        self.chip_type = ""
        self.module = module
        #optional
        self.is_dryrun = kwargs.get("is_dryrun", False)
        self.skip_download_bin = kwargs.get("skip_download_bin", False)
        self.triggered_by = kwargs.get("triggered_by", "")
        self.triggered_date = kwargs.get("triggered_date", "")
        self.is_smoke = kwargs.get("is_smoke", False)
        self.user_bin_test = kwargs.get("user_bin_test", False)
        self.src_path = os.path.join(os.getenv("WORKSPACE"), r"SRC")
        #self.src_path = os.path.join(r'D:\jenkins\workspace\Mouse', r"SRC")
        self.log_zip_queue = queue.Queue(50)
        self.process_info = {
            "pid_list": [],
            "pip_name_list": []
        }
    

    def getSessionID(self):
        print("------------------------query session start------------------------")
        res = subprocess.Popen("query session", shell=True, stdout=subprocess.PIPE)
        output_lines = res.stdout.readlines()
        decoded_lines = [line.decode('gbk') for line in output_lines]
        allStr = ''
        run_id = ""
        for item in decoded_lines:
            if ">" in item:
                run_id = re.findall(f'.*\s+{os.environ.get("USERNAME")}\s+(\d+)\s+[\u4e00-\u9fa5]+',item)[0]
            allStr = allStr + item
        print(allStr)
        print("------------------------query session stop------------------------")
        return run_id
    

    def run_test(self):
        res = False
        interval = 50 if self.test_type == 'Nightly' else 15
        retry_count = 1

        gui_exe = os.path.join(self.src_path, r"{}\bin\{}".format(self.module, "win32_sim"), 'gui.exe')

        for i in range(retry_count):
            log_path = log.get_log_dir(module = self.module)
            pip_name = log.get_pip_name(project=self.project ,module=self.module)
            print("pip name", pip_name)
            log_parameter_to_hrl = f'-honeygui {gui_exe}'
            if self.test_type in ['Nightly']:
                log_parameter_to_hrl += ""

            hrs_tool_info = slave_property_parse(self.project, self.branch, self.chip_type, 'HoneyRobotLight', 'role', 'ALL')
            print(hrs_tool_info['path'])

            hrs_pip_name = ""
            if self.test_type in ['Nightly']:
                hrs_pip_name = self.project + "_" + self.module + "_HoneyRobotLight"
            
            """
            if get_slave_property_value(self.chip_type, "need_tscon"):
                run_id = self.getSessionID()
                tool_base = ToolBase(cmd_line=[r'tscon', run_id, r'/v', r'/dest:console'], wait=False, timeout=100, delay=60)
                try:
                    tool_base.tool_run()
                except Exception as e:
                    print("tscon run fail: {}".format(e))
                self.getSessionID()
            """

            hrs_tool = HoneyRobotLight(project=self.project, branch=self.branch, chip_type=self.chip_type,
                                    test_type=self.test_type, sequence_index=self.sequence_index,
                                    tool_info=hrs_tool_info, log_path=log_path, test_module=self.module,
                                    exit_when_failed=False,
                                    is_dryrun=self.is_dryrun, interval=1, triggered_by=self.triggered_by,
                                    triggered_data=self.triggered_date, is_smoke=self.is_smoke,
                                    extra_global_parameter=log_parameter_to_hrl, pip_name=hrs_pip_name)

            res = hrs_tool.honeyrobotlight_run()
            time.sleep(5)

        if not res:
            sys.exit('{} test failure'.format(self.module))
            