#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
import sys
import re


def change_or_revert_macros(repo, app_flag_path, update_type, macro_list, insert=False):
    if update_type == "change" and not macro_list:
        print("There is no macro to change, skip")
        return

    if update_type == 'change':
        with open(app_flag_path, mode='r', newline='', errors='surrogateescape') as fd:
            lines = fd.readlines()
 
        with open(app_flag_path, mode='w+', newline='', errors='surrogateescape') as fd:
            for macro, value, check_pattern in macro_list:
                start_replace = False if check_pattern else True
                pattern = r'^(\s*#define\s+{}\s+)(\d+)'.format(macro)
                for i, line in enumerate(lines):
                    if check_pattern and re.search(check_pattern, line):
                        print("Find target definition, start change macro {}".format(macro))
                        start_replace = True
                    if start_replace:
                        if insert:
                            lines[i] = line + "#undef {}\n#define {}\t{}\n".format(macro, macro, value)
                            start_replace = False
                        else:
                            if re.search(pattern, line):
                                lines[i] = re.sub(pattern, lambda objs:objs.group(1)+value, line, count=1, flags=re.M)
                                start_replace = False
            fd.writelines(lines)
        
        print('Configure {} diff ->\n{}'.format(os.path.basename(app_flag_path), repo.git.diff(app_flag_path)), flush=True)
        
    elif update_type == 'revert':
        repo.git.checkout('--', app_flag_path)
        print('after revert {} diff ->\n{}'.format(os.path.basename(app_flag_path), repo.git.diff(app_flag_path)), flush=True)