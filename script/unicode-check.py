#!/usr/bin/env python

import sys
import os


if (len(sys.argv)) == 1:
    target_path = r'sdk' # parse sdk directory by default
else:
    target_path = sys.argv[1]

for dirpath, _, filenames in os.walk(os.path.join(os.getcwd(), target_path)):
    for name in filenames:
        if name.endswith(('.h', '.c', '.md')):
            with open(os.path.join(dirpath, name), 'rb') as f:
                #print(os.path.join(dirpath, name))
                for i, line in enumerate(f, 1):
                    try:
                        line.decode('utf-8')
                        line.decode('gbk')
                    except UnicodeDecodeError as e:
                        print('File: {}, Line: {}, Offset: {}, {}'.format(os.path.join(dirpath, name), i, e.start + 1, e.reason))
