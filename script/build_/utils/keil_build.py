import os
import subprocess
import xml.etree.ElementTree as ET
import shutil
from build_.utils.helper import *
import time
import re

class RequireTerminateException(Exception):
    pass


class KeilBuildBase():
    @staticmethod
    def find_target_for_project(project_path):
        target_list = []
        tree = ET.parse(project_path)
        targetname_tag_list = tree.findall('Targets/Target/TargetName')
        for i in targetname_tag_list:
            target_list.append(i.text)
        return target_list


    @staticmethod
    def check_project_compiling(project_path, keil_path=None, target=None, error_record_path="", ignore_warnings = False, retry_count=5):
        '''Check project compiling status:
           #0 No Errors or Warnings
           #1 Warnings Only
           #2 Errors
           #3 Fatal Errors
           #11 Cannot open project file for writing
           #12 Device with given name in not found in database
           #13 Error writing project file
           #15 Error reading import XML file
        '''
        if not os.path.exists(project_path):
            print("{} not exist".format(project_path))
            return False

        for i in range(retry_count):
            result = True
            log_file = r'output_compiling_{}.txt'.format(time.strftime("%y_%m_%d_%H_%M_%S", time.localtime()))
            cmd_compiling = keil_path or r'C:\Keil_v5\UV4\UV4.exe'
            cmd_compiling = [cmd_compiling] + [r'-r', project_path, r'-j0', r'-o', log_file]

            if target:
                cmd_compiling.extend(['-t', target])
            else:
                cmd_compiling.extend(['-z'])

            print('check project compiling: command {}'.format(cmd_compiling), flush=True)
            try:
                subprocess.check_output(cmd_compiling, universal_newlines=True, stderr=subprocess.STDOUT)
            except subprocess.CalledProcessError as e:
                result = False

            with open(os.path.dirname(project_path) + r'\{}'.format(log_file), errors='surrogateescape', encoding='utf-8') as f:
                if not result:
                    err_log = f.read()
                    if "Failed to move or copy license cache file from" in err_log or "Your license" in err_log:
                        print("Keil copy license cache file fail {} times(s), retry".format(i + 1))
                        continue

                    if ignore_warnings:
                        error_summary_list = re.findall(r'(\d+)\s*Error\(s\),\s*(\d+)\s*Warning\(s\)', err_log)
                        if error_summary_list:
                            for e_summary in error_summary_list:
                                errors = int(e_summary[0], base=0)
                                warnings = int(e_summary[1], base=0)
                                print("found log: {} errors, {} warnings".format(errors, warnings))
                                if errors > 0:
                                    print('build error detected, fail the build')
                                    break
                            else:
                                print("No errors found, all are warnings")
                                print('check project compiling: {} output with warnings ->\n{}'.format(project_path, err_log), flush=True)
                                return True

                    if error_record_path:
                        keywords = os.path.basename(project_path)
                        if target:
                            keywords = keywords + ":" + target
                        else:
                            keywords = keywords + ":z"

                        KeilBuildBase.__keep_keil_error_record(keywords,
                                                            os.path.dirname(project_path) + r'\{}'.format(log_file),
                                                            Helper.get_file_name_without_ext(project_path),
                                                            error_record_path)
                    print('check project compiling: {} output error ->\n{}'.format(project_path, err_log), flush=True)
                else:
                    print('check project compiling: {} output pass'.format(project_path), flush=True)

            return result
        else:
            print("Keil copy license cache file fail {} times".format(retry_count))
            return False

    @staticmethod
    def compile_projects(base_path, fail_fast, keil_path, error_record_path, excludes=[]):
        '''Iterate the base path to compile all projects'''
        if not os.path.exists(base_path):
            print("{} not exists.".format(base_path))
            return False

        base_path = os.path.abspath(base_path)
        result = True
        try:
            for dir_path, _, file_names in os.walk(base_path):
                for file_name in file_names:
                    if file_name.endswith(r'.uvprojx') and file_name not in excludes:
                        file_dir = os.path.join(dir_path, file_name)
                        if not KeilBuildBase.__build_single_project(project_path=file_dir, keil_path=keil_path,
                                                      raise_exception=fail_fast, target=None,
                                                      error_record_path=error_record_path):
                            result = False
        except RequireTerminateException as e:
            print("Already build failed, stop the build process")
            result = False

        return result


    @staticmethod
    def __build_single_project(project_path, target, keil_path, raise_exception, error_record_path,retry_count=5):
        if not KeilBuildBase.check_project_compiling(project_path=project_path,
                                       target=target,
                                       keil_path=keil_path,
                                       error_record_path=error_record_path,
                                       retry_count=retry_count):
            if raise_exception:
                raise RequireTerminateException(
                    "Build {} failed, need to terminate the build process".format(os.path.basename(project_path)))
            else:
                return False
        else:
            return True

    @staticmethod
    def build_single_project(project_path, target, keil_path, error_record_path,retry_count=5):
        return KeilBuildBase.__build_single_project(project_path=project_path, target=target, keil_path=keil_path,
                                      raise_exception=False, error_record_path=error_record_path,retry_count=retry_count)

    @staticmethod
    def build_project_list(project_list, fail_fast, keil_path, error_record_path, callback=None):
        result = True
        try:
            for prj_path, target in project_list:
                if not KeilBuildBase.__build_single_project(project_path=prj_path, target=target, keil_path=keil_path,
                                              raise_exception=fail_fast, error_record_path=error_record_path):
                    result = False
                    if callable(callback):
                        callback(prj_path, target, False)
                else:
                    if callable(callback):
                        if not callback(prj_path, target, True):
                            print("callback return False")
                            result = False
                            if fail_fast:
                                raise RequireTerminateException("build call back failed")
        except RequireTerminateException as e:
            print(e)
            print("Already build failed, stop the build process")
            result = False

        return result


    @staticmethod
    def __keep_keil_error_record(keywords, log_file, log_file_prefix, error_record_path):
        os.makedirs(error_record_path, exist_ok=True)
        dest_file_name = os.path.basename(log_file)
        dest_file_name = log_file_prefix + "-" + dest_file_name if log_file_prefix else dest_file_name
        shutil.copy2(log_file, os.path.join(error_record_path, dest_file_name))
        with open(os.path.join(error_record_path, "error_detail"), encoding='utf-8', mode='a') as fd:
            fd.write("{}->{}\n".format(keywords, dest_file_name))

    @staticmethod
    def find_all_keil_projects(base_path, excludes=[]):
        '''Iterate the base path find the projects'''
        if not os.path.exists(base_path):
            print("{} not exists.".format(base_path))
            return []

        project_list = list()
        base_path = os.path.abspath(base_path)


        for dir_path, _, file_names in os.walk(base_path):
            for file_name in file_names:
                if file_name.endswith(r'.uvprojx') and file_name not in excludes:
                    project_list.append(os.path.join(dir_path, file_name))

        return project_list
