import os
import shutil

__all__ = ('Helper',)

class Helper():
    def __init__(self):
        pass

    @staticmethod
    def convert_item_to_item_list(item):
        item = item if isinstance(item, list) else [item]
        return item

    @staticmethod
    def string_in_array_item(test_str,items, ignore_case):
        if not items:
            items = list()
        if ignore_case:
            return list(filter(lambda x: test_str.lower() in x.lower(), items))
        else:
            return list(filter(lambda x: test_str in x, items))

    @staticmethod
    def get_file_name_without_ext(file_path):
        return os.path.splitext(os.path.basename(file_path))[0]

    @staticmethod
    def find_file_path(base_path, file_name):
        #print("find_file_path  ===> ", base_path)
        '''Find the first matched file's under base path'''
        if not os.path.exists(base_path):
            return None

        for dir_path, _, file_names in os.walk(base_path):
            if file_name in file_names:
                file_dir = os.path.join(dir_path, file_name)
                #print('Find file {}'.format(file_dir), flush=True)
                return file_dir

        return None


    @staticmethod
    def create_dir(dir_path, remove_before_create=False):
        if remove_before_create:
            if os.path.exists(dir_path):
                shutil.rmtree(dir_path)

        os.makedirs(dir_path, exist_ok=True)




