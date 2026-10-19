import os
import sys
import time
import queue
import threading
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import jenkins_test_target_config
from test.slave_property import *
from filelock import FileLock
from base.parse_json import ParseJson
from base.tool_base import ToolBase
from peripheral.bee1_controller import Bee1Controller
from test.common_func import get_lock_file
from test.error_record import record_error
from download.image_map import DOWNLOAD_INFO

class MPCLIDownload():
    def __init__(self, project, branch, chip_type, role, module, part_number, download_info_json, **kwargs):

        self.project = project
        self.branch = branch
        self.chip_type = chip_type
        self.role = role
        self.module = module
        self.part_number = part_number
        self.download_info_json = download_info_json
        #optional
        self.timeout = kwargs.get("timeout", None)
        self.test_type = kwargs.get("test_type", "Nightly")
        self.skip_download = kwargs.get("skip_download", False)
        self.user_bin_test = kwargs.get("user_bin_test", False)
        self.mpcli = slave_property_parse(self.project, self.branch, self.chip_type, 'MPCLI', 'role', self.role, part_number=self.part_number)
        self.result_queue = None
        self.change_src_address = kwargs.get("change_src_address", False)
        jenkins_test_target_config._init(self.chip_type)
        self.MODULE_GROUP_MAP = jenkins_test_target_config.get_value("MODULE_GROUP_MAP")
        self.MODULE_APP_MAP = jenkins_test_target_config.get_value("MODULE_APP_MAP")

        #if not os.path.exists(self.download_info_json) and not self.skip_download:
        #    sys.exit("Cannot find {}".format(self.download_info_json))


    def mpcli_download(self, only_erase, download_all = True):
        if not os.path.exists(self.download_info_json) and not self.skip_download and not only_erase:
            sys.exit("Cannot find {}".format(self.download_info_json))

        com_number = self.mpcli['COM']
        mpcli_path = self.mpcli['path']

        #erase
        if download_all:
            addr_str = DOWNLOAD_INFO[self.chip_type]["Address"]
            size_str = '{}'.format(int(DOWNLOAD_INFO[self.chip_type]["Size"], base = 0))
            if self.chip_type == "rtl87x3d" and self.part_number == "RTL8773DFL":
                size_str = '{}'.format(int(int(DOWNLOAD_INFO[self.chip_type]["Size"], base = 0)/2))
            if self.test_type == "Release" and self.chip_type == "rtl87x3d":
                addr_str = str(hex(int(DOWNLOAD_INFO[self.chip_type]["Address"],16) + int("0x2000", 16))) #skip first 8K(authentication)
                size_str = str(hex(int(DOWNLOAD_INFO[self.chip_type]["Size"],16) - int("0x2000", 16)))
            cmd = [mpcli_path, '-c', com_number,'-e', '-A', addr_str,'-S', size_str,'-r', '-u', '-M', '5']
            if self.mpcli.get("type", "") == 'python':
                cmd = ['python'] + cmd

            self.result_queue = queue.Queue(10000) 
            thread_erase = threading.Thread(target=self.run_mpcli_mp_loader_cus, kwargs={"wait_timeout": 5})
            thread_erase.start()

            tool_base = ToolBase(cmd_line=cmd, exit_once_failed=False,timeout=self.timeout,  wait_event= {"message": "run into mp loader", "callback": self.run_mpcli_catch_mp_call_back})
            erase_res = tool_base.tool_run()
            self.result_queue.queue.clear()

            if not erase_res:
                return erase_res

        if only_erase:
            return erase_res

        #download
        mpcli_config = ParseJson.load_json_file(file_name = self.download_info_json)
        mpcli_config["mptoolconfig"]["port"] = com_number
        ParseJson.write_json_file(json_content = mpcli_config, file_name = os.path.basename(self.download_info_json), dir_name=os.path.dirname(self.download_info_json))
        bt_address = self.mpcli['btAddress']
        self.MODULE_APP_MAP[self.module]['cut_roles'][self.role]['address'] = bt_address

        if len(mpcli_config["mptoolconfig"]["appimage"]["file"]) == 0:
            print("Need to download full bin, and no default bin found in sdk.")
            return False
        image_info = mpcli_config["mptoolconfig"]["appimage"]["file"]
        for i in range(len(image_info)):
            print("[bin] {}, [address] {}".format(image_info[i]["name"], image_info[i]["address"]))

        time.sleep(1)
        if self.mpcli.get("type", "") == 'python':
            cmd = ['python']
        else:
            cmd = []
        cmd.extend([mpcli_path,
            r'-c', com_number,
            r'-f', self.download_info_json,
            r'-a', r'-M',r'5', r'-r'])

        bt_string = list(map(lambda x: x[2:], bt_address.split(":")))
        bt_string = ':'.join(bt_string)
        cmd.extend(['-x', bt_string])

        if self.mpcli.get("type", "") == 'python':
            cmd.append('-u')
            cmd.append('-d')

        self.result_queue = queue.Queue(10000) 
        thread_download = threading.Thread(target=self.run_mpcli_mp_loader_cus, kwargs={"wait_timeout": 5})
        thread_download.start()

        tool_base = ToolBase(cmd_line=cmd, exit_once_failed=False,timeout=self.timeout,  wait_event= {"message": "run into mp loader", "callback": self.run_mpcli_catch_mp_call_back})
        download_bin_res = tool_base.tool_run()
        self.result_queue.queue.clear()

        #after steps
        if self.change_src_address:
            dongle_mpcli = slave_property_parse(self.project, self.branch, self.chip_type, 'MPCLI', 'role', "DUT1", part_number=self.part_number)
            dongle_address = ':'.join(list(map(lambda x: x[2:], dongle_mpcli['btAddress'].split(":"))))
            cmd = [mpcli_path, '-c', com_number,'-m', dongle_address, '-A', '{}'.format(DOWNLOAD_INFO[self.chip_type]["SRCAddress"]),'-r', '-M', '5']
            if self.mpcli.get("type", "") == 'python':
                cmd = ['python'] + cmd

            self.result_queue = queue.Queue(10000) 
            thread_erase = threading.Thread(target=self.run_mpcli_mp_loader_cus, kwargs={"wait_timeout": 5})
            thread_erase.start()

            tool_base = ToolBase(cmd_line=cmd, exit_once_failed=False,timeout=self.timeout,  wait_event= {"message": "run into mp loader", "callback": self.run_mpcli_catch_mp_call_back})
            erase_res = tool_base.tool_run()
            self.result_queue.queue.clear()

        return download_bin_res


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
            bee1_controller = Bee1Controller(self.project, self.branch, self.chip_type, self.role, self.module, self.part_number)
            bee1_controller.test_cut_restart(interval=0)


    def download_bin(self, only_erase, pin_low_before_reset = False, download_all = True):
        if self.skip_download:
            return
        retry_count = 3
        lock_file_name = get_lock_file()
        with FileLock(lock_file_name, timeout=3600):
            Bee1Controller(self.project, self.branch, self.chip_type, 'IMPACT', self.module, self.part_number).test_cut_disable_all_controller_GPIO_pins()
            res = True
            print("download_bin: download for {}".format(self.role))
            for i in range(retry_count):
                self.download_bin_list = []
                bee1_controller = Bee1Controller(self.project, self.branch, self.chip_type, self.role, self.module, self.part_number)
                bee1_controller.test_cut_disable_all_controller_GPIO_pins()
                if pin_low_before_reset:
                    bee1_controller.test_cut_reset("reset_pre")
                # Download DUT Bin files through mpcli
                res = self.mpcli_download(only_erase, download_all)
                if pin_low_before_reset:
                    bee1_controller.test_cut_reset("reset_after")
                if not res:
                    continue
                else:
                    break
            else:
                record_error("DonwloadBinError: {} failed {} time(s)".format(self.role, retry_count))
                sys.exit('download_bin for {} failed {} time(s)'.format(self.role, retry_count))