import os
import sys
import json
import platform
import glob
from base.parse_json import ParseJson


def slave_property_parse(project, branch, chip_type, entry, key, value, count = 1):
    '''Parse slave property json file'''

    base_path = ParseJson.get_base_path()
    slave_property_name = os.path.join(base_path, 'SlaveProperty-{}.json'.format(chip_type))
    if not os.path.exists(slave_property_name):
        slave_property_name = os.path.join(base_path, 'SlaveProperty.json')
    print("slave property name: {}".format(slave_property_name))
    data = ParseJson.load_json_file(slave_property_name)
    #branch = branch + r'-' + CHIP_TYPE

    item_list = None
    for config in data['configurations']:
        if config['project'] == project:
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


def get_local_SlaveProperty(project, branch, chip_type, part_number, test_type):
    if not part_number:
        sys.exit("Please set the part number")

    base_path = ParseJson.get_base_path()
    slave_property_name = os.path.join(base_path, 'SlaveProperty-{}.json'.format(chip_type))
    if not os.path.exists(slave_property_name) and test_type in ["Nightly"]:
        slave_property_name = os.path.join(base_path, f'SlaveProperty_{os.environ.get("week_day")}.json')
    if not os.path.exists(slave_property_name):
        slave_property_name = os.path.join(base_path, 'SlaveProperty.json')

    print("slave property name: {}".format(slave_property_name))
    #branch = branch + r'-' + CHIP_TYPE
    return os.path.abspath(slave_property_name), project, branch


def get_slave_property_role_list(project, branch, chip_type, entry, part_number):
    slave_property_name = get_slave_property_file(part_number, chip_type)
    data = ParseJson.load_json_file(slave_property_name)
    role_list = list()
    for config in data['configurations']:
        if config['project'] == project:
            for br in config['branches']:
                if br['name'] == branch:
                    if entry not in br:
                        return role_list
                    for item in br[entry]:
                        role_list.append(item["role"])

    return role_list


def get_slave_property_file(chip_type):

    base_path = ParseJson.get_base_path()
    slave_property_name = os.path.join(base_path, 'SlaveProperty-{}.json'.format(chip_type))
    if not os.path.exists(slave_property_name):
        slave_property_name = os.path.join(base_path, 'SlaveProperty.json')
    print("slave property name: {}".format(slave_property_name))

    return slave_property_name


def get_all_com_in_config():

    base_path = ParseJson.get_base_path()
    slave_property_list = glob.glob(os.path.join(base_path, 'SlaveProperty*.json'))
    all_com = []
    for slave_property in slave_property_list:
        data = ParseJson.load_json_file(slave_property)
        item_list = None
        for config in data['configurations']:
            for br in config['branches']:
                for entry in br:
                    if entry == "name":
                        continue
                    for item in br[entry]:
                        if item.get("COM", ""):
                            if item["COM"] in all_com:
                                continue
                            else:
                                all_com.append(item["COM"])
                        elif  item.get("portName", ""):
                            if item["portName"] in all_com:
                                continue
                            else:
                                all_com.append(item["portName"])
    return all_com


def get_slave_property_config_item(project, branch, chip_type, part_number, entry, key, test_type):
    '''Parse slave property json file'''
    slave_property_name, project, branch = get_local_SlaveProperty(project, branch, chip_type, part_number, test_type)
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


def get_slave_property_value(chip_type, key):
    '''Parse slave property json file'''
    slave_property_name = get_slave_property_file(chip_type)
    data = ParseJson.load_json_file(slave_property_name)
    if key in data:
        print("key: {}".format(data[key]))
        return data[key]
    else:
        return None

