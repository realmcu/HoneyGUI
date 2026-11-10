import os
import sys
import re
import json
import platform
from base.parse_json import ParseJson


def slave_property_parse(project, branch, chip_type, entry, key, value, part_number, count = 1, module=None):
    '''Parse slave property json file'''
    slave_property_name = get_slave_property_file(part_number, chip_type)
    data = ParseJson.load_json_file(slave_property_name)

    item_list = None
    for config in data['configurations']:
        if config['project'] == project:
            br_list = list(map(lambda x: x['name'], config['branches']))
            c_branch = r'{}-{}'.format(branch, part_number)
            branch = c_branch if c_branch in br_list else branch
            c_branch = r'{}-crb314-framework'.format(branch)
            branch = c_branch if c_branch in br_list else branch
            print("slave property branch name: {}".format(branch))
            for br in config['branches']:
                if br['name'] == branch:
                    if entry not in br:
                        return None
                    for item in br[entry]:
                        if item[key] == value:
                            if count == 1:
                                return item
                            else:
                                if item_list == None:
                                    item_list = []
                                if count == 0 or len(item_list) < count:
                                    item_list.append(item)
                                else:
                                    return item_list

    return item_list

def get_slave_property_file(part_number, chip_type):
    if not part_number:
        sys.exit("Please set the part number")

    base_path = ParseJson.get_base_path()
    slave_property_name = os.path.join(base_path, 'SlaveProperty-{}.json'.format(chip_type))
    if not os.path.exists(slave_property_name):
        slave_property_name = os.path.join(base_path, 'SlaveProperty.json')
    print("slave property name: {}".format(slave_property_name))

    return slave_property_name

def get_slave_property_value(part_number, chip_type, key):
    '''Parse slave property json file'''
    slave_property_name = get_slave_property_file(part_number, chip_type)
    data = ParseJson.load_json_file(slave_property_name)
    if key in data:
        print("key: {}".format(data[key]))
        return data[key]
    else:
        return None


def get_slave_property_role_list(project, branch, chip_type, entry, part_number, module=None):
    slave_property_name = get_slave_property_file(part_number, chip_type)
    data = ParseJson.load_json_file(slave_property_name)
    role_list = list()
    for config in data['configurations']:
        if config['project'] == project:
            br_list = list(map(lambda x: x['name'], config['branches']))
            c_branch = r'{}-{}'.format(branch, part_number)
            branch = c_branch if c_branch in br_list else branch
            c_branch = r'{}-crb314-framework'.format(branch)
            branch = c_branch if c_branch in br_list else branch
            for br in config['branches']:
                if br['name'] == branch:
                    if entry not in br:
                        return role_list
                    for item in br[entry]:
                        role_list.append(item["role"])

    return role_list


def get_mpcli_erase_list(mpcli_role_list, cut_roles):
    return list(set(mpcli_role_list) - set(cut_roles))


def get_local_SlaveProperty(project, branch, chip_type, part_number):
    if not part_number:
        sys.exit("Please set the part number")

    base_path = ParseJson.get_base_path()
    slave_property_name = os.path.join(base_path, 'SlaveProperty-{}.json'.format(chip_type))
    if not os.path.exists(slave_property_name):
        slave_property_name = os.path.join(base_path, 'SlaveProperty.json')
    print("slave property name: {}".format(slave_property_name))

    if part_number in ['RTL8763EWE', 'RTL8763EW']:
        branch = r'{}-{}'.format(branch, part_number)
    if part_number in ['RTL8763ESE'] and re.search(r"feature\/bb2-sdk-v1-\d+-sprint\d+.*", os.environ['UPSTREAM_BRANCH']): #only for feature/sprint branch, ci test dial
        branch = "{}-sprint-headset".format(branch)
    return os.path.abspath(slave_property_name), project, branch


def get_slave_property_config_item(project, branch, chip_type, part_number, entry, key):
    '''Parse slave property json file'''
    slave_property_name, project, branch = get_local_SlaveProperty(project, branch, chip_type, part_number)
    data = ParseJson.load_json_file(slave_property_name)
    print(f"will load {slave_property_name}")

    item_list = None
    for config in data['configurations']:
        if config['project'] == project:
            for br in config['branches']:
                if br['name'] == branch:
                    if entry not in br:
                        print(f"{entry} not found")
                        return None
                    if key not in br[entry]:
                        print(f"{key} not found")
                        return None
                    return br[entry][key]

    return None
