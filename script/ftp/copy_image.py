# -*-coding:utf-8-*-
import os
import sys
import shutil
import glob
import re
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from ftp.archive_config import ARCHIVE_MAP
from ftp.ftp_helper import *


class CopyImage():
    @staticmethod
    def copy_image_to_ftp(work_dir, root_path, relative_path, zip_base_name):
        pwd = os.getcwd()
        os.chdir(work_dir)
        #zip image and copy to ftp

        zip_name = "{}.tar.gz".format(zip_base_name)
        shutil.make_archive(base_name=zip_base_name, format='gztar', root_dir=os.path.join(root_path, relative_path))
        #connect ftp
        hsot, port, key = rs_ftp_host()
        sftp = SFTP(hsot, port, key)
        sftp.connect()
        sftp.upload(os.path.join(work_dir, zip_name), rs_ftp_server())

        sftp.unzip(os.path.join(rs_ftp_server(local=True), zip_name), relative_path)
        sftp.disconnect()
        os.chdir(pwd)

