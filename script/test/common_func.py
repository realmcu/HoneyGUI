import os
import sys
import glob
import time
import subprocess
import shutil
sys.path.append(os.path.join(os.path.dirname(__file__), "..\\base"))
from base.parse_json import ParseJson

def get_lock_file():
    t_lock_file_name = "bee1.lock"
    slave_base_path = ParseJson.get_base_path()
    return os.path.join(slave_base_path, t_lock_file_name)