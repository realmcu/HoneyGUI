import sys
import os
from pathlib import Path
import xml.etree.ElementTree as ET
import argparse
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from build.sdk_build_base import *
from build.config_parse import *


class SDKBuild(SDKBuildBase):
    def __init__(self, yaml_path, west_home, chip_type, local_submodule_path=dict()):
        super(SDKBuild, self).__init__(yaml_path, west_home, chip_type, local_submodule_path)
        self.yaml_path = yaml_path
        self.west_home = west_home
        self.build_proj_lib_in_order = {
            "mandatory": [],
            "temporary": []
        }
        self.chip_type = chip_type

        self.config_parse = CfgParser()
        self.config_data = self.config_parse.parse_build_config()


    def get_all_build_project_list(self, prj_type, title, all=True):
        '''
            all: build all project
            fail_fast: fail the project if one project build fail
            keil_path: keil path on your system
        '''
        build_list = list()
        build_list.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"][f"{prj_type}_mandatory"]])
        if all:
            build_list.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"][f"{prj_type}_temporary"]])

        return build_list

    def get_io_project_list(self, ignore_warning, error_record_path, start_from_last_fail, only_build_app,keil_path, fail_fast, callback=None):
        build_lib = list()
        build_lib.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"]["keil_mandatory"]])
        sample_path = self.samples_path()
        io_path = os.path.join(sample_path, "peripheral")
        module_path = os.path.join(sample_path, "module_sample")
        print("io_path: ", io_path)
        print("module_path: ", module_path)
        for prj_path in [io_path, module_path]:
            for pj_dir, _, files in os.walk(prj_path):
                for file in files:
                    if file.endswith(r'.uvprojx'):
                        if any(t.replace("\\", "/") in os.path.join(pj_dir, file).replace("\\", "/") for t in self.config_data["build"]["base_lib_config"]["ignored_app"]):
                            print(f"ignore {os.path.join(pj_dir, file)}")
                            continue
                        build_lib.extend([(os.path.join(pj_dir,file), "", "honeygui")])
        
        project_list = self.convert_prj_relative_path_to_abs_path(build_lib)
        keil_build_project_list = [(a, b) for a, b , c in project_list]
        print(keil_build_project_list)

        if start_from_last_fail:
            prj, target = self.get_last_failure(error_record_path)
            print("last failure: {} {}".format(prj, target))
            keil_build_project_list = self.get_build_prj_list_from_required(keil_build_project_list, prj, target)
        
        if error_record_path:
            Helper.create_dir(error_record_path)
            detail_file, detail_file_exist = self.get_error_detail_file(error_record_path)
            if detail_file_exist:
                print("remove", detail_file)
                os.remove(detail_file)
    
        return KeilBuildBase.build_project_list(keil_build_project_list, fail_fast, keil_path, error_record_path,callback=callback)
    

    def get_exclude_project_from_app_config(self,exclude_prj, module_build_path):
        app_config_data = self.config_data["app_config"]
        chip_app_project_list = self.get_chip_app_project_list(app_config_data, module_build_path, app_key=None)
        for f in os.listdir(os.path.join(self.config_parse.build_config_path, "app_config")):
            proj_name = r"{}.uvprojx".format(os.path.splitext(f)[0])
            if os.path.splitext(f)[0] not in chip_app_project_list:
                continue
            exclude_prj.append(proj_name)
            target_map = app_config_data[os.path.splitext(f)[0]]["target_map"]
            all_lib_proj = list(map(lambda x: target_map[x]["libs"], target_map.keys()))
            for libs in all_lib_proj:
                exclude_prj.extend(list(map(lambda l: os.path.basename(l[0]), libs)))
        
        return exclude_prj

    def get_chip_app_project_list(self, app_config_data, module_build_path, app_key):
        chip_app_project_list = list()
        for proj in app_config_data:
            if app_key and proj != app_key: #match app
                continue
            if module_build_path and not self.filter_app_in_module_build(module_build_path, app_config_data[proj]["project_path"]):
                continue

            chip_app_project_list.append(proj)
        
        return chip_app_project_list
    

    def filter_app_in_module_build(self, module_build_path, prj_path):
        #filter app config not belong this module
        if len(list(filter(lambda x: x not in prj_path, module_build_path))) != len(module_build_path):
            return True #appconfig in module build path
        else:
            return False


    def app_target_need_add_to_build_list(self, target, project_name, build_list):
        if list(filter(lambda x: x[1]==target and os.path.basename(x[0])==os.path.basename(project_name), build_list)):
            return False
        return True


    def lib_target_need_build_again(self, target, project_name, build_list):
        build_list = list(filter(lambda x: os.path.basename(project_name) in x[0], build_list))
        if build_list and build_list[-1][1] == target:
            return False
        return True


    def get_single_proj_build_list_from_app_config(self, app_config_data, proj, build_list):
        if not app_config_data.get(proj):
            print("{} not in app config list, return".format(proj))
            return build_list
        proj_path = app_config_data[proj]["project_path"]
        app_config_targets = list()
        project_name = os.path.join(proj_path, r"{}.uvprojx".format(proj))
        
        for t_m in app_config_data[proj]["target_map"]:
            libs = app_config_data[proj]["target_map"][t_m]["libs"]
            libs = list(filter(lambda p: self.lib_target_need_build_again(p[1], p[0], build_list), libs))
            libs = list(map(lambda l: tuple(l), libs))
            targets = list(set(list(map(lambda b: b["target"], app_config_data[proj]["target_map"][t_m]["bank_info"].values()))))
            targets = list(filter(lambda t: self.app_target_need_add_to_build_list(t, project_name, build_list), targets))
            app_config_targets += targets
            app_list = list(map(lambda t: tuple([project_name, t, "honeygui"]), targets))
            if targets:
                build_list.extend(libs)
                build_list.extend(app_list)
        return build_list


    def get_build_prj_list_from_app_config(self, build_list, module_build_path, app_key=None):
        app_config_data = self.config_data["app_config"]
        chip_app_project_list = self.get_chip_app_project_list(app_config_data, module_build_path, app_key)
        for proj in chip_app_project_list:
            build_list = self.get_single_proj_build_list_from_app_config(app_config_data, proj, build_list)
        
        return build_list


    def get_keyword_project_list(self,exact, folder, ignore_warning, error_record_path, start_from_last_fail, only_build_app,keil_path):
        keil_build_lib = list()
        if self.config_data["build"]["base_lib_config"].get(f"{folder}_keil_mandatory"):
            keil_build_lib.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"][f"{folder}_keil_mandatory"]])
        else:
            keil_build_lib.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"]["keil_mandatory"]])

        gcc_build_list = list()
        if self.config_data["build"]["base_lib_config"].get(f"{folder}_gcc_mandatory"):
            gcc_build_list.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"][f"{folder}_gcc_mandatory"]])
        else:
            gcc_build_list.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"]["gcc_mandatory"]])

        for dir in folder.split("/"):
            if self.config_data.get("app_config") and dir in self.config_data["app_config"]:
                keil_build_lib = self.get_build_prj_list_from_app_config(keil_build_lib, app_key=dir, module_build_path=[])
            else:
                sdk_path = self.sdk_path()
                app_check_dir_list = ["applications", "samples", "tests"]
                for app_check_dir in app_check_dir_list:
                    app_check_dir = Path(sdk_path).joinpath(app_check_dir)
                    app_dir_list = list(app_check_dir.glob(f"**\{dir}")) if exact else list(app_check_dir.glob(f"**\{dir}*"))
                    for app_dir in app_dir_list:
                        for pj_dir, _, files in os.walk(str(app_dir)):
                            for file in files:
                                if any(t.replace("\\", "/") in os.path.join(pj_dir, file).replace("\\", "/") for t in self.config_data["build"]["base_lib_config"]["ignored_app"]):
                                        print(f"ignore {os.path.join(pj_dir, file)}")
                                        continue
                                if file.endswith(r'.uvprojx'):
                                    keil_build_lib.extend([(os.path.join(pj_dir,file), "", "honeygui")])
                                elif file == "build_all_target.sh":
                                    gcc_build_list.extend([(os.path.join(pj_dir, "build_all_target.sh"), "", "honeygui")])
                                    break
                                elif file == "Makefile" and not os.path.exists(os.path.join(pj_dir, "build_all_target.sh")):
                                    gcc_build_list.extend([(pj_dir, "", "honeygui")])

        keil_project_list = self.convert_prj_relative_path_to_abs_path(keil_build_lib)
        keil_build_project_list = [(a, b) for a, b , c in keil_project_list]
        for k in keil_build_project_list:
            print(k)

        if start_from_last_fail:
            prj, target = self.get_last_failure(error_record_path)
            print("last failure: {} {}".format(prj, target))
            keil_build_project_list = self.get_build_prj_list_from_required(keil_build_project_list, prj, target)
        
        if error_record_path:
            Helper.create_dir(error_record_path)
            detail_file, detail_file_exist = self.get_error_detail_file(error_record_path)
            if detail_file_exist:
                print("remove", detail_file)
                os.remove(detail_file)
    
        for t_pname, t_ptarget in keil_build_project_list:
            if not KeilBuildBase.check_project_compiling(project_path=t_pname, target=t_ptarget, ignore_warnings=ignore_warning,keil_path=keil_path):
                return False
        
        gcc_project_list = self.convert_prj_relative_path_to_abs_path(gcc_build_list)
        gcc_build_project_list = [(a, b) for a, b , c in gcc_project_list]
        for g in gcc_build_project_list:
            print(g)

        for t_pname, t_ptarget in gcc_build_project_list:
            if not GCCBuildBase.check_gcc_project_compiling(project_path=t_pname,target=t_ptarget):
                return False

        return True


    def build_app_by_project_name(self, project_name, target_name, ignore_warning, error_record_path, start_from_last_fail, only_build_app,keil_path):
        project_name_simple = project_name.split(".")[0]
        build_lib = list()
        build_lib.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"]["keil_mandatory"]])
        project_name = project_name_simple + ".uvprojx"

        if project_name_simple in self.config_data['app_config'] and target_name:
            for part_number in self.config_data["app_config"][project_name_simple]["target_map"]:
                taraget_list = self.config_data["app_config"][project_name_simple]["target_map"][part_number]["bank_info"].values()
                taraget_list = [t["target"] for t in taraget_list]
                if target_name in taraget_list:
                    build_lib.extend([tuple(x) for x in self.config_data["app_config"][project_name_simple]["target_map"][part_number]["libs"]])

        build_lib = list(dict(zip([os.path.basename(x[0]) for x in build_lib], build_lib)).values())
        build_lib.append((project_name, "", "honeygui"))
        project_list = self.convert_prj_relative_path_to_abs_path(build_lib)
        keil_build_project_list = [(a, b) for a, b , c in project_list]
        
        if start_from_last_fail:
            prj, target = self.get_last_failure(error_record_path)
            print("last failure: {} {}".format(prj, target))
            keil_build_project_list = self.get_build_prj_list_from_required(keil_build_project_list, prj, target)
        
        if error_record_path:
            Helper.create_dir(error_record_path)
            detail_file, detail_file_exist = self.get_error_detail_file(error_record_path)
            if detail_file_exist:
                print("remove", detail_file)
                os.remove(detail_file)
    
        for t_pname, t_ptarget in keil_build_project_list:
            if not KeilBuildBase.check_project_compiling(project_path=t_pname, target=t_ptarget, ignore_warnings=ignore_warning,keil_path=keil_path):
                return False

        return True   


help_str = r"""
    run in HoneyComb root dir
    cmd examples:

        build all project

        [build all project, continue to build when one project build fail]
            python script/build/sdk_build.py -chipType rtl87x3g --keilPath C:\Keil_v5\UV4\UV4.exe --all
        [build all project, if one project build fail, exit the process]
            python script/build/sdk_build.py -chipType rtl87x3g --failFast --keilPath C:\Keil_v5\UV4\UV4.exe --all
        [build all project, start from the project build failed last time]
            python script/build/sdk_build.py -chipType rtl87x3g --lf --keilPath C:\Keil_v5\UV4\UV4.exe --all

        build app
        when build app, process will be exited when one keil project fail 

        [build one project with project name and target name(build depend lib first)]
            python script/build/sdk_build.py -chipType rtl87x3g --project dongle_rtl87x3g.uvprojx -t target
        [build one project with project name and target name(build depend lib first), start from last fail] 
            python script/build/sdk_build.py -chipType rtl87x3g --project dongle_rtl87x3g.uvprojx -t target --lf
        [build one project with project name and target name(do not build lib)]
            python script/build/sdk_build.py -chipType rtl87x3g --project dongle_rtl87x3g.uvprojx -t target --onlyApp
"""


if __name__ == '__main__':
    print("Run build in {}".format(os.getcwd()))
    parser = argparse.ArgumentParser(description="", usage=help_str)
    parser.add_argument('-chipType', default = 'rtl87x3g', help='Set the chip type')
    parser.add_argument('--all', action = 'store_true', help='build all project')
    parser.add_argument('--failFast', action = 'store_true', help='stop build process when one project build fail')
    parser.add_argument('--keilPath', default = None, help='absolute UV4.exe path')
    parser.add_argument('--error_record_path', default = '', help='if copy keil error record to error_record_path')
    parser.add_argument('--lf', action = 'store_true', help='start to build from last failure')
    parser.add_argument('-p', '--project', help='set the project you want to build')
    parser.add_argument('--onlyApp', action = 'store_true', help='onlyBuildApp')
    parser.add_argument('-t', '--target', default=None, help='set the target you want to build')
    parser.add_argument('--ignore', action = 'store_true', help='ignore build warnings')

    arg_dict = parser.parse_args()
    if arg_dict.project:
        if not arg_dict.target:
            sys.exit("Please set the target for project {}".format(arg_dict.project))
        if not SDKBuild(yaml_path=os.environ[yaml_path_env], west_home=os.environ[westHome_env],chip_type=arg_dict.chipType,local_submodule_path=local_submodule_path).build_app_by_project_name(project_name=arg_dict.project, target_name=arg_dict.target, ignore_warning=arg_dict.ignore, error_record_path=arg_dict.error_record_path, start_from_last_fail=arg_dict.lf, only_build_app=arg_dict.onlyApp, keil_path=arg_dict.keilPath):
            sys.exit("Build project fail")
        else:
            print("Build project pass")
            sys.exit(0)
    
    if not arg_dict.error_record_path:
        arg_dict.error_record_path = os.path.join(os.getcwd(), "keil_build_record_{}".format(arg_dict.chipType))
    
    if not SDKBuild(yaml_path=os.environ[yaml_path_env], west_home=os.environ[westHome_env],chip_type=arg_dict.chipType,local_submodule_path=local_submodule_path).build_all_keil_projects(title="honeygui", fail_fast=arg_dict.failFast, keil_path=arg_dict.keilPath, all=arg_dict.all, error_record_path=arg_dict.error_record_path,start_from_last_fail=arg_dict.lf):
        sys.exit("Build project fail")
    else:
        print("Build project pass")
