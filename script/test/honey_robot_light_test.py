#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import sys
import time
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import log
import jenkins_test_target_config
from slave_property import *
from common_func import *
from filelock import FileLock
from tool.honey_robot_light import HoneyRobotLight
from peripheral.bee1_controller import Bee1Controller
from download.mpcli_download import MPCLIDownload


class HoneyRobotLightTest():
    def __init__(self):
        pass

    def run_test(self):
        pass
