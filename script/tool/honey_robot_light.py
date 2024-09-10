import os
import sys
import shutil
import time
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import jenkins_test_target_config
from base.tool_base import ToolBase
from test.slave_property import *
from test.common_func import *
from tool.tool_func import *


class HoneyRobotLight(ToolBase):
    def __init__(self, project, branch, chip_type, log_path, tool_info, 
                sequence_index, test_type, test_module, **kwargs):
        self.project = project
        self.branch = branch
        self.chip_type = chip_type
        self.log_path = log_path
        self.tool_info = tool_info
        self.sequence_index = sequence_index
        self.test_type = test_type
        self.test_module = test_module
        #optional
        self.exit_when_failed = kwargs.get("exit_when_failed", True)
        self.is_dryrun = kwargs.get("is_dryrun", False)
        self.interval = kwargs.get("interval", 0)
        self.triggered_by = kwargs.get("triggered_by", "")
        self.triggered_date = kwargs.get("triggered_date", "")
        self.is_smoke = kwargs.get("is_smoke", False)
        self.full_pack_bin = kwargs.get("full_pack_bin", "")
        self.extra_global_parameter = kwargs.get("extra_global_parameter", "")
        self.pip_name = kwargs.get("pip_name", "")


    def get_action_group(self):
        self.base_group_name = r'actions_{}'.format(self.test_module)
        self.group_name_suffix = ""
        if "HRL_Tool" in self.triggered_by:
            if self.is_dryrun == True:
                self.group_name_suffix = r"_dryrun"
            else:
                self.group_name_suffix = r"_CI"
        else:
            self.group_name_suffix = r"_{}".format(self.test_type)
            if self.test_type == "FreeStyle":
                self.group_name_suffix = r"_smoke" if self.is_smoke else r"_Nightly"
            if self.test_type == 'Release':
                if self.is_dryrun:
                    self.group_name_suffix = r"_dryrun"
                elif self.is_smoke:
                    self.group_name_suffix = r"_smoke"
            if self.test_type == "CI":
                self.group_name_suffix = r"_dryrun"
            if self.test_type == "Nightly":
                if self.is_dryrun:
                    self.group_name_suffix = r"_dryrun"
        if int(self.sequence_index) > 0:
            self.group_name_suffix += r"_{}".format(self.sequence_index)

        return self.base_group_name + self.group_name_suffix
    

    def get_hrl_test_tool_sequence_info(self, origin_sequence_path):
        # only used to test hrs tool
        test_sequence_path = os.path.join(os.environ['WORKSPACE'], 'HoneyRobotLight', origin_sequence_path[origin_sequence_path.index("Scripts") : ])
        tool_path = os.path.join(os.environ['WORKSPACE'], 'HoneyRobotLight','Tool','HoneyRobotLight.exe')
        return (tool_path, test_sequence_path)
    

    def honeyrobotlight_run(self):
        '''Run HoneyRobotLight tool.'''
        tool_path = self.tool_info['path']
        script_path = os.path.abspath(os.path.join(os.path.dirname(tool_path), "../Scripts"))
        result = True
        delete_xml = False
        action_group_name = self.get_action_group()

        if not action_group_name in self.tool_info:
            if self.test_type == "FreeStyle":
                print("FreeStyle use the device as {}, please configure dryrun first".format(action_group_name))
                return False
            else:
                print("honeyrobotlight_run: {}-{} has no testsuit group {}".format(self.project, self.branch, action_group_name))
                return True
        else:
            print("action_group_name: {}".format(action_group_name))
        
        self.log_path = self.log_path or "log"
        honeyRobotLight_log_path = os.path.join(self.log_path, 'HRL')

        if os.path.exists(honeyRobotLight_log_path):
            shutil.rmtree(honeyRobotLight_log_path)

        android_device_ids = {}
        for action in self.tool_info[action_group_name]:
            if 'devices' in action:
                json_path = action['devices']
                with open(json_path, "r", encoding = 'utf-8') as f:
                    dev_dict = json.load(f)
                for p in dev_dict.get("PhoneDevs", []):
                    if "AndroidID" in p:
                        android_device_ids[p["AndroidID"]] = android_device_ids.get(p["AndroidID"], "")
                        if "DevName" in p:
                            android_device_ids[p["AndroidID"]] = p["DevName"]
        print("all adb devices: {}".format(list(android_device_ids)))

        for action in self.tool_info[action_group_name]:
            sequence_path = action.get('script', "") or action.get('sequence', "")
            if not sequence_path:
                print("Empty script and sequence config")
                return False
            if "HRL_Tool" in self.triggered_by:
                '''need_run_action = self.honeyrobotlight_run_pre_check()
                if not need_run_action:
                    print("Skip test, for HRL Tool change has on influence with sequence {}".format(os.path.basename(sequence_path)))
                    continue'''
                tool_path, sequence_path = self.get_hrl_test_tool_sequence_info(sequence_path)
            
            elif self.test_type == "FreeStyle":
                delete_xml = False
                sequence_path = generate_xml_for_freeStyle(test_module=self.test_module, tool_path=tool_path, orig_sequence_path=sequence_path)
            print(sequence_path)
            if not os.path.exists(sequence_path):
                return False
            
            #for role in MODULE_APP_MAP[self.test_module]['cut_roles']:
                #lock_file_name = get_lock_file()
                #with FileLock(lock_file_name, timeout=600):
                    #bee1_controller = Bee1Controller(self.project, self.branch, self.chip_type, role, self.test_module, self.part_number)
                    #bee1_controller.test_cut_restart(interval=1)
            
            time.sleep(5)
            cmd = [tool_path]
            sequence_param = "-script" if action.get("script", "") else '-sequence'
            cmd.extend([sequence_param, sequence_path, '-log', honeyRobotLight_log_path])
            if action.get("devices"):
                cmd.extend(['-devicesparams', action['devices']])

            global_string = ''
            if os.environ.get('Global_Parameter'): # only freestyle has this env
                global_string = os.environ.get('Global_Parameter').strip() + ";"
            if 'global' in action:
                global_string += action['global'] + ';'
                global_string = global_string.strip(';') + ";" + self.extra_global_parameter.strip(';')
                cmd.extend(['-global', global_string.strip(';')])
            if self.extra_global_parameter:
                cmd.extend([self.extra_global_parameter])

            if self.test_type == 'CI' or self.triggered_by == "HRL_Tool_CI":
                cmd.append('-exitOnceFailed')
            cmd.append('-savewavpath')
            if self.test_type in ['CI', "FreeStyle"]:
                cmd.append(honeyRobotLight_log_path)
            elif "HRL_Tool" in self.triggered_by:
                cmd.append(os.path.join(os.path.dirname(self.tool_info['path']), r"../WaveFile"))
            else:
                cmd.append(os.path.join(os.path.dirname(tool_path), r"../WaveFile"))
            
            if self.pip_name:
                cmd.extend(['-pipename', self.pip_name])
            
            if self.test_type == "FreeStyle":
                if os.getenv('EXIT_ONCE_FAILED', 'false') == 'true':
                    cmd.append('-exitOnceFailed')
                action_parameter = "TC_Parameter"
                if os.getenv(action_parameter,''):
                    for line in os.environ[action_parameter].split('\n'):
                        if len(line.strip()) == 0:
                            continue

                        if len(line.split(' ')) < 2:
                            print("tc name and parameter should be separated by space")
                            return False
                        cmd.extend(['-action', line.split(' ')[0], ' '.join(line.split(' ')[1:])])
            
            tool_base = ToolBase(cmd_line=cmd, delay=self.interval, exit_once_failed=self.exit_when_failed)
            res = tool_base.tool_run()
            if not self.exit_when_failed:
                if not res:
                    result = False
        
        return result
