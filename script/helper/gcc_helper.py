import os
import sys
import re
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from log import *

def update_defconfig(file_path, key, value, count=0):
    with open(file_path, mode='r', newline='', errors='surrogateescape') as fd:
        stream = fd.read()
        if re.search(r"\s*{}\s*=\s*".format(key), stream):
            LOG_DEBUG("Find {} macro definition, start replace macro value".format(key))
            stream = re.sub(r'^(\s*{}\s*=\s*)([^\n#]*)'.format(key), lambda objs:objs.group(1)+value, stream, count=count, flags=re.M)
        else:
            LOG_DEBUG("{} macro definition not found, adding new one".format(key))
            if not stream.endswith('\n'):
                stream += '\n'
            stream += "{}={}\n".format(key, value)

    with open(file_path, mode='w+', newline='', errors='surrogateescape') as fd:
        fd.write(stream)