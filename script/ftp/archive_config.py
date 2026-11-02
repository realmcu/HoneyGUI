#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

ARCHIVE_MAP = {
    "Common": {
        "script": [(r"DependTools/Python Lib/ImageInfo/imageInfo.py", "local"),
                   (r"DependTools/Python Lib/FileLock/filelock.py", "local"),
                   (r"DependTools/Python Lib/Bee1PinControl/bee1control.py", "local"),
                   (r"script/jenkins-test.py", "sdk"),
                   (r"script/jenkins_test_target_config.py", "sdk"),
                   (r"script/log", "sdk"),
                   (r"script/ftp", "sdk"),
                   (r"script/test", "sdk"),
                   (r"script/report", "sdk"),
                   (r"script/base", "sdk"),
                   (r"script/SDKPlatformConfig", "sdk"),
                   (r"IntegrationTester", "Integration")]
    }
}
