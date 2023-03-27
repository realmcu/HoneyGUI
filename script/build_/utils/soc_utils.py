import re

class SOCUtils():

    @staticmethod
    def check_dup_log_index(filename):
        print('check_dup_log_index: {}'.format(filename))
        ret = True
        log_index_line_map = {}
        #pattern = re.compile("#define\s*\w+\s*(\d+).*")
        pattern = re.compile(r'#define\s+\w+\s+([0-9a-fA-FxX]{1,}).*')
        with open(filename, 'r') as fp:
            for line_num, line in enumerate(fp, 1):
                m = pattern.match(line)
                if m is not None:
                    try:
                        log_index = int(m.group(1), base=0)
                        if log_index in log_index_line_map:
                            sys.stderr.write(
                                "check_dup_log_index: log index {} is duplicate: line {}, line {}\n".format(
                                    log_index, log_index_line_map[log_index],
                                    line_num))
                            ret = False
                        else:
                            log_index_line_map[log_index] = line_num
                    except Exception as e:
                        print('check_dup_log_index: line {}: {}'.format(line_num, line))
        return ret

    
    @staticmethod
    def IsSourceFileModified(file_list):
        for f in file_list:
            if not f.endswith(('.txt', '.md', '.gitignore')):
                return True
        return False
