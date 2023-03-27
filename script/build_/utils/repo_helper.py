import os
import xml.dom.minidom


__all__ = ('RepoHelper',)
class RepoHelper():
    LOWERSTACK_ROM_GROUP_NAME = "lowerstack-rom"
    LOWERSTACK_PATCH_GROUP_NAME = "lowerstack-patch"
    SYS_PATCH_GROUP_NAME = "sys-patch"
    FRAMEWORK_GROUP_NAME = "framework"
    SDK_GROUP_NAME = "sdk"
    ASSISTANT_SDK_GROUP_NAME = "assistant-sdk"
    SCRIPT_GROUP_NAME = "script"
    ROM_GROP_NAME = "rom"
    UPPER_GROP_NAME = "upper"

    @staticmethod
    def get_module_path(honeyrepo_root_dir, sub_git_name):
        if not os.path.exists(honeyrepo_root_dir):
            return None

        for dir_path, _, file_names in os.walk(honeyrepo_root_dir):
            if "{}-build.py".format(sub_git_name) in file_names:
                return os.path.abspath(os.path.join(dir_path, ".."))

        return None


    @staticmethod
    def get_all_proj_form_manifest_file(manifest_file_path):
        # do not support include
        remotes = dict()
        default = dict()
        all_project = list()
        root = xml.dom.minidom.parse(manifest_file_path)
        for manifest in root.childNodes:
          if manifest.nodeName == 'manifest':
            break
        else:
            raise Exception("no <manifest> in {}".format(manifest_file_path))
        for node in manifest.childNodes:
            if node.nodeName == 'remote':
                r_name = node.getAttribute("name")
                r_url = node.getAttribute("fetch")
                remotes[r_name] = {
                    "name": r_name,
                    "url": r_url
                }
            if node.nodeName == 'default':
                default["remote"] = node.getAttribute('remote')
        for node in manifest.childNodes:
            if node.nodeName == 'project':
                p_name = node.getAttribute('name')
                p_remote = node.getAttribute('remote') or default["remote"]
                p_remote = remotes[p_remote]["url"]
                p_path = node.getAttribute('path')
                p_revision = node.getAttribute('revision')
                p_branch = node.getAttribute('dest-branch')
                if node.hasAttribute('groups'):
                    p_groups = node.getAttribute('groups')
                else:
                    p_groups = ""
                p_groups = p_groups.split()
                all_project.append(
                    {
                        "name": p_name,
                        "path": p_path,
                        "revision": p_revision,
                        "branch": p_branch,
                        "url": p_remote,
                        "groups": p_groups
                    }
                )
        return all_project


    @staticmethod
    def get_submodule_info_by_group_name(all_proj_info, submoule_group_name):
        for p in all_proj_info:
            if p["groups"] and p["groups"][0] == submoule_group_name:
                return p


    @staticmethod
    def get_prj_real_path_in_repo(honeyrepo_root_dir, prj_relative_path, sub_git_name):
        #print(honeyrepo_root_dir, prj_relative_path, sub_git_name)
        subgit_root_dir = RepoHelper.get_module_path(honeyrepo_root_dir, sub_git_name)
        return os.path.abspath(os.path.join(subgit_root_dir, prj_relative_path))
