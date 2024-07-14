import os
import sys
import shutil
import glob
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from ftp.ftp_helper import *


def get_log_dir(module):
    log_path = os.path.join(os.getcwd(), 'log', module)
    return log_path


def record_test_info(module_name, result, log_path):
    dest_json = os.path.join(os.getcwd(), r'log\report', r'testReport-{}.json'.format(module_name))
    os.makedirs(os.path.dirname(dest_json), exist_ok=True)

    data = dict()
    key = "{}".format(module_name)
    if not data.get(key):
        data[key] = dict()
    data[key]["result"] = result
    data[key]["slave"] = os.getenv("NODE_NAME")
    data[key]["bin"] = os.path.join(os.path.dirname(log_path), "bin")
    data[key]["log"] = log_path

    ParseJson.write(dest_json, data)

    return dest_json


def copy_log_to_ftp(work_dir, src_path, module_name):
    if not os.path.exists(src_path):
        print("{} doesn't exist, skip copy log to ftp".format(src_path))
        return
    try:
        log_zip = f"{module_name}_log.tar.gz"
        cwd = os.getcwd()
        os.chdir(work_dir)
 
        #connect ftp
        hsot, port, key = get_ftp_host()[0]
        sftp = SFTP(hsot, port, key)
        abs_log_path = "log"
        unpack_path = f"{module_name}/{abs_log_path}"
        try:
            shutil.make_archive(base_name=os.path.splitext(os.path.splitext(log_zip)[0])[0], format='gztar', root_dir=src_path)
            os.chdir(cwd)
            sftp.connect()
            sftp.upload(os.path.join(work_dir, log_zip), get_ftp_folder()[0])
            if module_name in ["HoneyGUI_CMD"]:
                print("upload platform result")
                test_report = glob.glob(os.path.join(os.environ.get("WORKSPACE"), r'SRC\report', r"HoneyGUI_*.json"))[0]
                sftp.upload(test_report, module_name)
            sftp.unzip(os.path.join(get_ftp_folder(local=True)[0], log_zip), unpack_path)
            sftp.disconnect()
            os.remove(os.path.join(work_dir, log_zip))
        except Exception as e:
            print("zip failed: {}, upload log folder".format(e))
            sftp.connect()
            sftp.upload(src_path, unpack_path)
            sftp.disconnect()
        finally:
            os.chdir(cwd)

    except Exception as e:
        print("Copy test log to ftp folder {} failed: {}".format(unpack_path, e))

