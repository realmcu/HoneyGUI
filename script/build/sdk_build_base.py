import os
import shutil
import sys
import itertools
import subprocess
import json

default_yml_name = r"rtl87x3g-sdk-zephyr_ci.yml"
chip_type = 'rtl87x3g'
westHome_env = "WestHome"
yaml_path_env = "yaml_abs_path"
west_projects_env = "west_projects_json"


def __find_west():
    """Look for a west installation, starting at the current directory.
    return honeyrepo_root, build/utils
    """
    t_west = shutil.which('west')
    res = {
        "west_home": "",
        "JenkinsBuild_util_Dir": "",
        "local_submodule_path": None,
        "yaml_path": None
    }

    # no west installed, or no west topdir exist, set as sdk is cloned by self, not west
    if t_west is not None:
        try:
            west_home = subprocess.check_output(['west', 'topdir'], universal_newlines=True, stderr=subprocess.STDOUT)
            west_home = west_home.strip()
            res["west_home"] = west_home
        except subprocess.CalledProcessError as e:
            print('Not a west dir')
            res["west_home"] = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../../../../..'))
            res["JenkinsBuild_util_Dir"] = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../../script/build_/utils'))
            res["yaml_path"] = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../../../../../manifest/bb2ultra/rtl87x3g', default_yml_name))
            res["local_submodule_path"] = {
                "honeygui": os.path.abspath(os.path.join(os.path.dirname(__file__), r'../..')),
            }
            return res
        # this is a west dir, project can get by west list
        try:
            JenkinsScript_info = subprocess.check_output(['west', 'list', "jenkinsScript"], universal_newlines=True, stderr=subprocess.STDOUT)
            JenkinsScript_info = JenkinsScript_info.split("\n")
            JenkinsScript_info = list(filter(lambda o: o.strip(), JenkinsScript_info))
            JenkinsScript_info = JenkinsScript_info[0]
            JenkinsScript_info = JenkinsScript_info.split()
            JenkinsBuild_util_Dir = os.path.join(west_home, JenkinsScript_info[1], "utils")
            res["JenkinsBuild_util_Dir"] = JenkinsBuild_util_Dir
            return res
        except subprocess.CalledProcessError as e:
            JenkinsBuild_util_Dir = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../..', "script" ,"build_", "utils"))
            res["JenkinsBuild_util_Dir"] = JenkinsBuild_util_Dir
            return res
    else:
        print("No west install")
        res["west_home"] = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../../..'))
        res["JenkinsBuild_util_Dir"] = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../..', "script" ,"build_", "utils"))
        res["local_submodule_path"] = {
            "honeygui": os.path.abspath(os.path.join(os.path.dirname(__file__), r'../..'))
        }
        res["yaml_path"] = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../../../../../manifest/bb2ultra/rtl87x3g', default_yml_name))
        return res


local_submodule_path = None
if os.environ.get(westHome_env, None) is None:
    print("called from local")
    find_res = __find_west()
    print(json.dumps(find_res, indent=4))
    os.environ[westHome_env] = find_res["west_home"]
    if find_res["yaml_path"]:
        os.environ[yaml_path_env] = find_res["yaml_path"]
    JenkinsBuild_util_Dir = find_res["JenkinsBuild_util_Dir"]
    local_submodule_path = find_res["local_submodule_path"]
else:
    print("called in jenkins")
    print("current west home:{}".format(os.environ.get(westHome_env)))
    jenkinsBuild_home = os.environ.get("jenkinsScript_abspath")
    JenkinsBuild_util_Dir = os.path.join(jenkinsBuild_home, "utils")

print("JenkinsBuild_util_Dir", JenkinsBuild_util_Dir)
sys.path.append(JenkinsBuild_util_Dir)

from keil_build import *
from gcc_build import GCCBuildBase
from west_helper import *

if os.environ.get(yaml_path_env) is None:
    print("get ${yaml_path_env} from local west env")
    os.environ[yaml_path_env] = SocWestHelper().get_west_yaml_path()

print(yaml_path_env, os.environ[yaml_path_env])

if local_submodule_path is not None:
    print("submodule not set in west env, load it from yaml")
    submodule_info_in_yaml = SocWestHelper().convert_yaml_to_json(os.environ[yaml_path_env])['manifest']
    t_all_projects = submodule_info_in_yaml['projects']
    #print(json.dumps(t_all_projects, indent=4))
    for t_k in t_all_projects:
        if t_k['name'] == 'honeygui':
            continue
        local_submodule_path[t_k['name']] = os.path.join(os.environ[westHome_env], t_k['path'])


class SDKBuildBase(KeilBuildBase):
    def __init__(self, yaml_path, west_home, chip_type, local_submodule_path=dict(), **kwargs):
        self.all_manifest_project_info = dict()
        self.west_home = west_home
        self.local_submodule_path = local_submodule_path or dict()
        print(f"local_submodule_path: {self.local_submodule_path}")
        self.chip_type = chip_type


    def convert_prj_relative_path_to_abs_path(self, prj_item_list):
        if not isinstance(prj_item_list, list):
            prj_item_list = [prj_item_list]

        res = list()
        for prj_relative_path, target, subgit_group in prj_item_list:
            if self.local_submodule_path.get(subgit_group,""):
                prj_parent_dir = self.local_submodule_path[subgit_group]
            else:
                if subgit_group not in self.all_manifest_project_info:
                    self.all_manifest_project_info[subgit_group] = SocWestHelper.get_project_info_by_project_name(subgit_group)['path']
                prj_parent_dir = os.path.join(self.west_home, self.all_manifest_project_info[subgit_group])
            if not prj_parent_dir:
                prj_parent_dir = input("Please input subgit group path:")

            if os.path.basename(prj_relative_path) == prj_relative_path:
                prj_abs_path = Helper.find_file_path(prj_parent_dir, prj_relative_path)
            else:
                prj_abs_path = os.path.join(prj_parent_dir, prj_relative_path)

            res.append((prj_abs_path, target, subgit_group))
        return res


    def samples_path(self):
        return os.path.abspath(os.path.join(self.west_home, SocWestHelper.get_project_info_by_project_name('honeygui')['path'], r'.\samples'))


    def sdk_path(self):
        return os.path.abspath(os.path.join(self.west_home, SocWestHelper.get_project_info_by_project_name('honeygui')['path']))
    

    def get_error_detail_file(self, error_record_path):
        return os.path.join(error_record_path, "error_detail"), os.path.isfile(os.path.join(error_record_path, "error_detail"))


    def get_last_failure(self, error_record_path):
        detail_file, detail_file_exist = self.get_error_detail_file(error_record_path)
        if not detail_file_exist:
            print("No last failure record")
            return "", ""
        with open(detail_file) as fd:
            all_lines = fd.readlines()

        for line in range(len(all_lines)-1, -1, -1):
            if "->" in all_lines[line]:
                print(all_lines[line])
                prj, target = all_lines[line].split(r"->")[0].split(":")
                return prj, target
        return "", ""


    def get_build_prj_list_from_required(self, all_prj_list, prj, target):
        if not prj:
            print("No prj set, return all")
            return all_prj_list

        for i in range(len(all_prj_list)):
            if os.path.basename(all_prj_list[i][0]) == prj:
                i_target=all_prj_list[i][1] if all_prj_list[i][1] is not None else "z"
                if i_target==target:
                    return all_prj_list[i:]
        return all_prj_list


    def build_all_keil_projects(self, title, all=False, fail_fast=True, keil_path=None, error_record_path="", start_from_last_fail=False, callback=None):
        '''
            all: build all project
            fail_fast: fail the project if one project build fail
            keil_path: keil path on your system
        '''
        all_prj_list =  self.get_all_build_project_list(prj_type="keil", all=all, title=title)

        keil_patten_project_list = self.convert_prj_relative_path_to_abs_path(all_prj_list)
        keil_build_project_list = [(os.path.abspath(a), b) for a, b , c in keil_patten_project_list]

        if start_from_last_fail:
            prj, target = self.get_last_failure(error_record_path)
            print("last failure: {} {}".format(prj, target))
            keil_build_project_list = self.get_build_prj_list_from_required(keil_build_project_list, prj, target)

        if error_record_path:
            Helper.create_dir(error_record_path)
            detail_file, detail_file_exist = self.get_error_detail_file(error_record_path)
            if detail_file_exist:
                os.remove(detail_file)
        print("all keil_build_project")
        for k in keil_build_project_list:
            print(k)
        return KeilBuildBase.build_project_list(keil_build_project_list, fail_fast, keil_path, error_record_path,callback=callback)


    def build_all_gcc_projects(self, title, all=False, fail_fast=True):
        all_prj_list =  self.get_all_build_project_list(prj_type="gcc", all=all, title=title)
        gcc_project_list = self.convert_prj_relative_path_to_abs_path(all_prj_list)
        gcc_build_project_list = [(os.path.abspath(a), b) for a, b , c in gcc_project_list]

        print("all_gcc_build_project")
        for k in gcc_build_project_list:
            print(k)
        return GCCBuildBase.build_gcc_project_list(gcc_build_project_list, fail_fast)
