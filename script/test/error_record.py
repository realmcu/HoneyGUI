import os
import sys

def record_error(msg, need_exit=False):
    error_log_file = os.path.join(os.path.dirname(__file__), 'error.log')
    if not os.path.exists(error_log_file): 
        fd = open(error_log_file, mode='w', encoding='utf-8')
        fd.close()
    print(msg)
    try:
        with open(error_log_file, 'a') as f:
            f.write(msg)
    except Exception as e:
        print("Write file exception:{}".format(e))
    if need_exit:
        sys.exit(1)