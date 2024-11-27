import importlib
import copy

def _init(chip_type):
    jira_info_map = {
        "rtl87x2g": {
            'issue_key': '',
            "project": "BEE4BUG",
            "jira_issue_labels": [r'HRSAutoTest', r'Jenkins_to_be_Checked'],
            "cut_label": [r'RTL8762G_VB'],
            "freestyle_link": "https://cn4soc-jks.rtkbf.com/view/Freestyle/job/FreeStyle~HoneyComb~bee4-sdk-v1~B~Cut/",
            "issue_dict": {
                'project': "",
                'summary': "",
                'description': "",
                'labels': "",
                'issuetype': {'name': 'Issue'},
                'assignee': "",
                'customfield_10601': [{'value': 'General'}], # RTL NO
                'customfield_10602': [{'value': 'SoC'}], # Chip Type
                'customfield_10603': {'value': 'Others'},  # Function
                'customfield_10604': {'value': 'General'}, # OS
                'customfield_10605': {'value': 'Others'}, # Application
                'customfield_10606': [{'value': 'Others'}], # Customer
                'customfield_10609': "", # HW Information
                'customfield_10007': "", # Failure Rate
                'customfield_10008': "" # Reproduce Steps
            }
        }
    }
    part_jira_info_map = copy.deepcopy(jira_info_map)
    try:
        test_module_map = {
            "HoneyGUI_Monkey_Test": {
                "Nightly": {
                    "issue_key": "BEE4BUG-1733",
                    "owner": [r'liang_zhou']
                },
                "CI": {
                    "issue_key": "BEE4BUG-1768",
                    "owner": [r'liang_zhou']
                }
            }
        }
        del part_jira_info_map[chip_type]["issue_key"]
    except ModuleNotFoundError as e:
        test_module_map = {
            'Gap_Ble':{},
            'Gap_Le_Audio':{},
            'Platform':{},
            "OTA":{},
            "Rom_Le_Audio": {},
            "Platform_Power_LDO":{},
            "Platform_Power_PFM":{},
            "Mouse":{},
            "Matter": {},
            "DashBoard":{},
        }
        print(e)
    for module in test_module_map:
        test_module_map[module].update(part_jira_info_map[chip_type])

    return test_module_map, jira_info_map
