#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import copy
import json
import re


MODULE_APP_MAP = {
    "HoneyGUI_CMD": {
        'test_class_module': "auto_test"
    },
    "HoneyGUI_Simulator":{
        'test_class_module': "honeygui_simulator_test"
    }
}

ALL_SUPPORTED_MODULE = list(MODULE_APP_MAP.keys())
