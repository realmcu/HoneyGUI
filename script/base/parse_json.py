#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import sys
import json
import platform

class ParseJson():

    @staticmethod
    def read(file_name):
        if not os.path.exists(file_name):
            sys.exit("{} doesn't exists".format(file_name))
        try:
            with open(file_name) as f:
                data = json.load(f)
        except Exception as e:
            sys.exit('Load JSON file failed: {}'.format(e))

        return data

    @staticmethod
    def write(file_name, content):
        with open(file_name, mode='w+', newline='', errors='surrogateescape') as fd:
            json.dump(content, fd, indent=4)

    @staticmethod
    def get_base_path():
        dir_name = None
        if 'BASE' in os.environ and os.path.exists(os.path.join(os.environ['BASE'], 'local')):
            dir_name = os.path.join(os.environ['BASE'], 'local')
        else:
            platform_unme = platform.uname()
            if 'Linux' in platform_unme:
                dir_name = os.path.join(os.environ['HOME'], 'jenkins', 'local')
            else:
                dir_name = os.path.join(r'D:\jenkins', 'local')
        
        return dir_name
    
    @classmethod
    def load_json_file(cls, file_name, dir_name = None):
        if not dir_name:
            dir_name = cls().get_base_path()
        if not os.path.exists(os.path.join(dir_name, file_name)):
            return None
        data = cls().read(os.path.join(dir_name, file_name))

        return data

    @classmethod
    def write_json_file(cls, json_content, file_name, dir_name = None):
        if not dir_name:
            dir_name = cls().get_base_path()
        file_full_path = os.path.join(dir_name, file_name)
        cls().write(file_full_path, json_content)

        return True

    