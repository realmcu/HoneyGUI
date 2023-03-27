import sys
import os
import copy
import xml.etree.ElementTree as ET
import argparse
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from build.sdk_build_base import *
from build.config_parse import *


class SDKBuild(SDKBuildBase):
    def __init__(self, manifest_path, repo_home, chip_type, local_submodule_path=dict()):
        super(SDKBuild, self).__init__(manifest_path, repo_home, chip_type)
        self.manifest_path = manifest_path
        self.repo_home = repo_home
        self.build_proj_lib_in_order = {
            "mandatory": [],
            "temporary": []
        }
        self.chip_type = chip_type

        self.config_parse = CfgParser()
        self.config_data = self.config_parse.parse_build_config()
        self.local_submodule_path = local_submodule_path


    def get_all_build_project_list(self, all=True):
        '''
            all: build all project
            fail_fast: fail the project if one project build fail
            keil_path: keil path on your system
        '''
        build_list = list()
        build_list.extend(self.config_data["build"]["base_lib_config"][self.chip_type]["mandatory"])
        if all:
            build_list.extend(self.config_data["build"]["base_lib_config"][self.chip_type]["temporary"])

        return build_list
    

    def get_keyword_project_list(self,exact, folder, ignore_warning, error_record_path, start_from_last_fail, only_build_app,keil_path):
        build_lib = list()
        build_lib.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"][self.chip_type]["mandatory"]])
        faga_path = self.fpga_path()
        for dir_path,_,file_names in os.walk(faga_path):
            if (os.path.basename(dir_path) == folder and exact) or (folder in os.path.basename(dir_path) and not exact):
                    for file_name in file_names:
                        if file_name.endswith(r'.uvprojx'):
                            build_lib.extend([(os.path.join(dir_path,file_name), "", "sdk")])

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


    def build_app_by_project_name(self, project_name, target_name, ignore_warning, error_record_path, start_from_last_fail, only_build_app,keil_path):
        project_name_simple = project_name.split(".")[0]
        build_lib = list()
        build_lib.extend([tuple(x) for x in self.config_data["build"]["base_lib_config"][self.chip_type]["mandatory"]])
        project_name = project_name_simple + ".uvprojx"

        if project_name_simple in self.config_data['app_config'] and target_name:
            for part_number in self.config_data["app_config"][project_name_simple]["target_map"]:
                taraget_list = self.config_data["app_config"][project_name_simple]["target_map"][part_number]["bank_info"].values()
                taraget_list = [t["target"] for t in taraget_list]
                if target_name in taraget_list:
                    build_lib.extend([tuple(x) for x in self.config_data["app_config"][project_name_simple]["target_map"][part_number]["libs"]])

        build_lib = list(dict(zip([os.path.basename(x[0]) for x in build_lib], build_lib)).values())
        build_lib.append((project_name, "", "sdk"))
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

        
if __name__ == '__main__':
    print("Run build in {}".format(os.getcwd()))
    parser = argparse.ArgumentParser(description='do project build')
    parser.add_argument('-chipType', default = 'rtl87x2f', help='Set the chip type')
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
        if not SDKBuild(manifest_path=os.environ[manifest_path_env], repo_home=os.environ[honeyRepo_env],chip_type=arg_dict.chipType,local_submodule_path=local_submodule_path).build_app_by_project_name(project_name=arg_dict.project, target_name=arg_dict.target, ignore_warning=arg_dict.ignore, error_record_path=arg_dict.error_record_path, start_from_last_fail=arg_dict.lf, only_build_app=arg_dict.onlyApp):
            sys.exit("Build project fail")
        else:
            print("Build project pass")
            sys.exit(0)
    
    if not arg_dict.error_record_path:
        arg_dict.error_record_path = os.path.join(os.getcwd(), "keil_build_record_{}".format(arg_dict.chipType))
    
    if not SDKBuild(manifest_path=os.environ[manifest_path_env], repo_home=os.environ[honeyRepo_env],chip_type=arg_dict.chipType,local_submodule_path=local_submodule_path).build_all_keil_projects(fail_fast=arg_dict.failFast, keil_path=arg_dict.keilPath, all=arg_dict.all, error_record_path=arg_dict.error_record_path,start_from_last_fail=arg_dict.lf):
        sys.exit("Build project fail")
    else:
        print("Build project pass")

