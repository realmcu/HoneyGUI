import os
import json

class CfgParser(object):
    def __init__(self):
        self.build_config_path = os.path.dirname(os.path.abspath(__file__))
        self.build_config_dict = dict()


    def parse_build_config(self):
        for dir_path, _, file_names in os.walk(self.build_config_path):
            for f in file_names:
                if not f.endswith(r'.json'):
                    continue
                dir_name = os.path.basename(dir_path)
                if not self.build_config_dict.get(dir_name):
                    self.build_config_dict[dir_name] = dict()
                f_path = os.path.join(dir_path, f)
                f_name = os.path.splitext(os.path.basename(f_path))[0]
                if not self.build_config_dict[dir_name].get(f_name):
                    self.build_config_dict[dir_name][f_name] = dict()
                with open(os.path.join(self.build_config_path, f_path)) as fd:
                    data = json.load(fd)
                    self.build_config_dict[dir_name][f_name].update(data)
        
        return self.build_config_dict