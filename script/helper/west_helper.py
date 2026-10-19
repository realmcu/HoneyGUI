import os
import git
import glob
import subprocess
from build_flags import *
from helper.keil_helper import *
from helper.gcc_helper import *

def after_build(chaged_files, git_repo):
    if not chaged_files:
        return
    for f in chaged_files:
        try:
            git_repo.git.checkout("--", f)
        except Exception as e:
            print("failed revert(git checkout -- {}): {}".format(f, e))

def flatten_all(data):
    res = []
    if data == None:
        return res
    if isinstance(data, list):
        for item in data:
            res.extend(flatten_all(item))
    else:
        res.append(str(data))
    return res

def run_cmd(cmd, path, module, before_build_msg=None, save_log=False):
    if not before_build_msg:
        before_build_msg = []
    else:
        before_build_msg = flatten_all(before_build_msg)
    print(" ".join(cmd), flush=True)
    try:
        subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=True)
        return True
    except Exception as e:
        print("Call {} fail".format(" ".join(cmd)))
        print(e.output)
        if save_log:
            error_log_path = os.path.join(os.getcwd(), "error_log_{}.txt".format(module.replace("/", "_")))
            with open(error_log_path, "w", encoding="utf-8") as f:
                f.write("==> now build: {}\n==> path: {}\n==> cmd: {}\n==> before_build_msg: {}\n".format(module, path, " ".join(cmd), "\n".join(before_build_msg)))
                f.write("==> error log:\n {}".format(e.output))
        return False

def run_pre_bat(current_prj_path, cmd_list):
    build_result = {}
    err_log_file_list = []
    os.chdir(os.path.join(current_prj_path, "script"))
    print("now in folder: {}".format(os.getcwd()))
    for cmd in cmd_list:
        cmd_str = " ".join(cmd).split(".")[0]
        build_result[cmd_str] = [True, ""]
        if not run_cmd(cmd, os.getcwd(), cmd_str, save_log=True):
            build_result[cmd_str][0] = False
            build_result[cmd_str][1] += " ".join(cmd)
            err_log_file_list += glob.glob(os.path.join(os.getcwd(), 'error_log_*.txt'))
            return False, build_result, err_log_file_list
    return True, build_result, err_log_file_list

def build_all(current_prj_path, is_dryrun = False, except_project = None, only_project = None): #except_project and only_project should be list
    # os.chdir(current_prj_path)
    # print("now in folder: {}".format(os.getcwd()))
    build_repo = git.Repo(search_parent_directories=True)
    build_result = {}
    err_log_file_list = []
    for p in loop_macros_relation:
        if except_project:
            if p in except_project:
                continue
        if only_project:
            if p not in only_project:
                continue
        first_message = True
        for message in loop_macros_relation[p]:
            if is_dryrun and not loop_macros_relation[p][message].get("CI", False):
                continue
            if not is_dryrun and not loop_macros_relation[p][message].get("Nightly", True):
                continue
            if first_message:
                build_result[p] = [True, ""]
                os.chdir(os.path.join(current_prj_path, p))
                print("now in folder: {}".format(os.getcwd()))
                first_message = False
            chaged_files = []
            before_build_list = list()
            print("=========build {}=========".format(message))
            if "before_build" in loop_macros_relation[p][message]:
                before_build_info = loop_macros_relation[p][message]["before_build"]
                if "build_macro" in before_build_info:
                    before_build_list += before_build_info["build_macro"]
                    for m in before_build_info["build_macro"]:
                        chaged_files.append(m[2][0])
                        update_macro(os.path.join(current_prj_path, m[2][0]), m[0], m[1])
                        LOG_INFO('Configure {} diff ->\n{}'.format(os.path.basename(m[2][0]), build_repo.git.diff(m[2][0])))
                if "gcc_build_macro" in before_build_info:
                    before_build_list += before_build_info["gcc_build_macro"]
                    for m in before_build_info["gcc_build_macro"]:
                        chaged_files.append(m[2][0])
                        update_defconfig(os.path.join(current_prj_path, m[2][0]), m[0], m[1])
                        LOG_INFO('Configure {} diff ->\n{}'.format(os.path.basename(m[2][0]), build_repo.git.diff(m[2][0])))
                if "update_func" in before_build_info:
                    before_build_list += before_build_info["update_func"]
                    for m in before_build_info["update_func"]:
                        chaged_files.append(m[2][0])
                        update_func_string(os.path.join(current_prj_path, m[2][0]), m[0], m[1])
                        LOG_INFO('Configure {} diff ->\n{}'.format(os.path.basename(m[2][0]), build_repo.git.diff(m[2][0])))
                if "update_overlay_func" in before_build_info:
                    before_build_list += before_build_info["update_overlay_func"]
                    for m in before_build_info["update_overlay_func"]:
                        chaged_files.append(m[2][0])
                        update_overlay_func(os.path.join(current_prj_path, m[2][0]), m[0], m[1])
                        LOG_INFO('Configure {} diff ->\n{}'.format(os.path.basename(m[2][0]), build_repo.git.diff(m[2][0])))
                # if "build_cmd" in before_build_info:
                #     before_build_list += before_build_info["build_cmd"]
                #     cmd_list = before_build_info["build_cmd"]
                #     run_pre_bat(current_prj_path, cmd_list)
                #     os.chdir(os.path.join(current_prj_path, p))
            cmd = ["west", "build", "-b"] + loop_macros_relation[p][message]["target"].split(" ")
            try:
                if not run_cmd(cmd, p, message, before_build_msg=before_build_list, save_log=True):
                    build_result[p][0] = False
                    build_result[p][1] += ">>{}\r\n".format(message)
                    if is_dryrun:
                        return False, build_result, err_log_file_list
            except Exception as e:
                print("!!!Exception: {}".format(e))
                build_result[p][0] = False
                build_result[p][1] += ">>{}\r\n".format(message)
                if is_dryrun:
                    return False, build_result, err_log_file_list
            after_build(list(set(chaged_files)), build_repo)
        err_log_file_list += glob.glob(os.path.join(os.getcwd(), 'error_log_*.txt'))
    print("=========build result=========")
    for k in build_result:
        print("build {} cmd(s): {} {}".format(k, "pass" if build_result[k][0] else "fail", "" if build_result[k][0] else "\r\n{}".format(build_result[k][1])))
    return all(v[0] for v in build_result.values()), build_result, err_log_file_list
