# -*-coding:utf-8-*-

import os
import sys
import re
import shutil
import argparse
from ftp_helper import *

class CopyScript(object):
    
    def get_file_location_by_group(file_group):
        file_location = None
        if file_group == "local":
            return "D:/admin"
        if file_group == "sdk":
            return os.environ.get("HONEYGUI_ROOT")
        if file_group == "Integration":
            return os.environ.get("HoneyRepo_ROOT")
        return file_location
    
    def copy_files(dest_copy_folder, file_list):
        for (file_name, file_group) in file_list:
            file_location = CopyScript.get_file_location_by_group(file_group)
            src_file_name = os.path.join(file_location, file_name)
            if not os.path.exists(src_file_name):
                print("Warning: file {} doesn't exist".format(src_file_name))
                continue
            os.makedirs(dest_copy_folder, exist_ok=True)
            if file_group == "sdk" and re.split(r"\\|/", file_name)[0] == "script":
                file_split_list = re.split(r"\\|/", file_name)
                file_name = "\\".join(file_split_list[1:])
                if len(file_split_list[1:]) > 1:
                    os.makedirs(os.path.join(dest_copy_folder, "\\".join(file_split_list[1:-1])), exist_ok=True)
                if os.path.isfile(src_file_name):
                    shutil.copyfile(src_file_name, os.path.join(dest_copy_folder, file_name))
                else:
                    shutil.copytree(src_file_name, os.path.join(dest_copy_folder, file_name))
            else:
                if os.path.isfile(src_file_name):
                    shutil.copyfile(src_file_name, os.path.join(dest_copy_folder, os.path.basename(file_name)))
                else:
                    if file_group == "Integration":
                        shutil.copytree(src_file_name, os.path.join(dest_copy_folder, os.path.basename(file_name)))
                        print("copy tree", src_file_name, os.path.join(dest_copy_folder, os.path.basename(file_name)))
                    else:
                        shutil.copytree(src_file_name, os.path.join(dest_copy_folder))
                        
    
    def get_folder_file_map(folder_file_map, map_dict):
        for r_folder in map_dict:
            if not folder_file_map.get(r_folder):
                folder_file_map[r_folder] = []
            folder_file_map[r_folder].extend(map_dict[r_folder]) 

        return folder_file_map

    def parse_archive_config(test_type, cut_series):
        folder_file_map = dict()
        if ARCHIVE_MAP.get("Common"):
            #common
            folder_file_map = CopyScript.get_folder_file_map(folder_file_map, ARCHIVE_MAP["Common"])
        if ARCHIVE_MAP.get("cut_series", {}).get(cut_series):
            if ARCHIVE_MAP["cut_series"][cut_series].get("Common"):
                #cut series common
                folder_file_map = CopyScript.get_folder_file_map(folder_file_map, ARCHIVE_MAP["cut_series"][cut_series]["Common"])
            if ARCHIVE_MAP["cut_series"][cut_series].get("test_type", {}).get(test_type):
                folder_file_map = CopyScript.get_folder_file_map(folder_file_map, ARCHIVE_MAP["cut_series"][cut_series]["test_type"][test_type])
        
        return folder_file_map

    def copy_archive_files(copy_folder, test_type, cut_series, dest_folder):
        folder_file_map = CopyScript.parse_archive_config(test_type, cut_series)
        for f in folder_file_map:
            if f == dest_folder:
                CopyScript.copy_files(copy_folder, folder_file_map[f])


if __name__ == '__main__':
    print("Copy archive files to ftp")
    parser = argparse.ArgumentParser(description='Copy archive files')
    parser.add_argument('--testType', choices=['CI', 'Nightly', 'Release', 'FreeStyle'], help='Set the test type')
    parser.add_argument('--zipFolder', choices=['script', 'trace'], help='zip folder')
    parser.add_argument('--step', default='copyScript', choices=['copyScript', 'removeZip'])
    parser.add_argument('--localFolder', help='Test script temp local folder')
    parser.add_argument('--workDir', help='the workdir')

    arg_dict = parser.parse_args()
    hsot, port, key = rs_ftp_host()
    work_dir = arg_dict.workDir
    if arg_dict.step == 'copyScript':
        from archive_config import ARCHIVE_MAP
        os.chdir(work_dir)
        zip_name = "{}.tar.gz".format(arg_dict.localFolder)
        dest_copy_path = os.path.join(work_dir, os.path.splitext(os.path.splitext(zip_name)[0])[0])
        CopyScript.copy_archive_files(dest_copy_path, arg_dict.testType, None, arg_dict.zipFolder)
        shutil.make_archive(base_name=os.path.splitext(os.path.splitext(zip_name)[0])[0], format='gztar', root_dir=dest_copy_path)
        
        sftp = SFTP(hsot, port, key)
        sftp.connect()
        sftp.upload(os.path.join(work_dir, zip_name), rs_ftp_server())
        sftp.disconnect()
    else:
        ftp1_folder,  ftp2_folder = get_ftp_folder()
        host_info1, host_info2 = get_ftp_host()
        for ftp_folder, h in [(ftp1_folder, host_info1), (ftp2_folder, host_info2)]:
            sftp = SFTP(h[0], h[1], h[2])
            sftp.connect()
            try:
                sftp.sftp.chdir(ftp_folder)
            except:
                continue
            for f in sftp.sftp.listdir(ftp_folder):
                if os.path.splitext(f)[1] in [".gz", ".zip"]:
                    sftp.sftp.remove(f)
            sftp.disconnect()
