import os
import shutil
import sys
import itertools
from build_.utils.keil_build import *

default_manifest_name = r"dev.xml"
chip_type = 'rtl87x2f'

def __find_root():
    """Look for a repo installation, starting at the current directory.
    return honeyrepo_root, build/utils
    """
    curdir = os.path.abspath(os.path.dirname(__file__))
    repo_root_build = None
    olddir = None
    while curdir != olddir and not repo_root_build:
        repo_root_build = os.path.join(curdir, "build", "build.py")
        if not os.path.isfile(repo_root_build):
            repo_root_build = None
            olddir = curdir
            curdir = os.path.dirname(curdir)
    if repo_root_build:
        return (os.path.abspath(curdir), os.path.abspath(os.path.join(curdir, "build", "utils")))
    else:
        return None, None


honeyRepo_env = "HoneyRepo"
manifest_path_env = "manifest"
if not os.environ.get(honeyRepo_env, None):
    honeyRepo_home, honeyRepo_uitl_dir = __find_root()
    if not honeyRepo_home:
        # currently, RDs do not use repo, so copy the build subgit to build_
        honeyRepo_home = os.path.abspath(os.path.join(os.path.dirname(__file__), r'../..'))
        local_submodule_path={
            "sdk": os.path.abspath(os.path.join(os.path.dirname(__file__), r'..')),
            "framework": ""
        }
        honeyRepo_uitl_dir = os.path.join(local_submodule_path["sdk"], "script" ,"build_", "utils")
        os.environ[honeyRepo_env] = honeyRepo_home
        print("can not find repo home, set {}".format(os.environ[honeyRepo_env]))
    else:
        os.environ[honeyRepo_env] = honeyRepo_home
        print("find repo home {}".format(os.environ[honeyRepo_env]))
else:
    print("current repo home:{}".format(os.environ[honeyRepo_env]))
    honeyRepo_uitl_dir = os.path.join(os.environ[honeyRepo_env], "build", "utils")

if not os.environ.get(manifest_path_env, None):
    os.environ[manifest_path_env] = os.path.abspath(os.path.join(honeyRepo_uitl_dir, ".." ,default_manifest_name))

sys.path.append(honeyRepo_uitl_dir)
print(honeyRepo_env, os.environ[honeyRepo_env])
print("honeyRepo_uitl_dir", honeyRepo_uitl_dir)
print(manifest_path_env, os.environ[manifest_path_env])

from repo_helper import *
from keil_build import *
from ride_build import *

class SDKBuildBase(KeilBuildBase):
    def __init__(self, manifest_path, repo_home, chip_type, local_submodule_path=dict()):

        self.all_manifest_project_info = RepoHelper.get_all_proj_form_manifest_file(manifest_path)
        self.lowerstack_patch_info = RepoHelper.get_submodule_info_by_group_name(self.all_manifest_project_info, RepoHelper.LOWERSTACK_PATCH_GROUP_NAME)
        self.repo_home = repo_home
        self.local_submodule_path = local_submodule_path
        self.chip_type = chip_type


    def convert_prj_relative_path_to_abs_path(self, prj_item_list):
        if not isinstance(prj_item_list, list):
            prj_item_list = [prj_item_list]

        res = list()
        for prj_relative_path, target, subgit_group in prj_item_list:
            if self.local_submodule_path.get(subgit_group,""):
                prj_parent_dir = self.local_submodule_path[subgit_group]
            else:
                prj_parent_dir = os.path.join(self.repo_home, RepoHelper.get_submodule_info_by_group_name(self.all_manifest_project_info, subgit_group)['path'])
            if not prj_parent_dir:
                prj_parent_dir = input("Please input subgit group path:")

            if os.path.basename(prj_relative_path) == prj_relative_path:
                prj_abs_path = Helper.find_file_path(prj_parent_dir, prj_relative_path)
            else:
                prj_abs_path = os.path.join(prj_parent_dir, prj_relative_path)

            res.append((prj_abs_path, target, subgit_group))
        return res
    

    def evb_path(self):
        return os.path.abspath(os.path.join(self.repo_home, RepoHelper.get_submodule_info_by_group_name(self.all_manifest_project_info, 'sdk')['path'], r'.\sdk\board\evb'))
    

    def fpga_path(self):
        return os.path.abspath(os.path.join(self.repo_home, RepoHelper.get_submodule_info_by_group_name(self.all_manifest_project_info, 'sdk')['path'], r'.\sdk\board\fpga'))
    

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


    def build_all_keil_projects(self, all=False, fail_fast=True, keil_path=None, error_record_path="", start_from_last_fail=False, callback=None):
        '''
            all: build all project
            fail_fast: fail the project if one project build fail
            keil_path: keil path on your system
        '''
        all_prj_list =  self.get_all_build_project_list(all=all)

        keil_patten_project_list = self.convert_prj_relative_path_to_abs_path(all_prj_list)
        keil_build_project_list = [(a, b) for a, b , c in keil_patten_project_list]

        if start_from_last_fail:
            prj, target = self.get_last_failure(error_record_path)
            print("last failure: {} {}".format(prj, target))
            all_project_target_list = self.get_build_prj_list_from_required(all_project_target_list, prj, target)

        if error_record_path:
            Helper.create_dir(error_record_path)
            detail_file, detail_file_exist = self.get_error_detail_file(error_record_path)
            if detail_file_exist:
                os.remove(detail_file)

        return KeilBuildBase.build_project_list(keil_build_project_list, fail_fast, keil_path, error_record_path,callback=callback)

