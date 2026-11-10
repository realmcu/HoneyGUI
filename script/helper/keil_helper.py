import os
import sys
import re
from xml.dom.minidom import parse
import xml.etree.ElementTree as ET
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from log import *

def add_include_node(domTree, node_name, text):
    node =  domTree.createElement(node_name)
    commonProperty_node = domTree.createElement("CommonProperty")
    include_node = domTree.createElement("IncludeInBuild")
    include_text = domTree.createTextNode(str(text))
    include_node.appendChild(include_text)
    commonProperty_node.appendChild(include_node)
    node.appendChild(commonProperty_node)

    return node


def read_xml(file_name):
    if not os.path.exists(file_name):
        sys.exit("File {} doesn't exists".format(file_name))
    domTree = parse(file_name)

    return domTree


def write_xml(domTree, file_name):
    try:
        with open(file_name, 'w', newline='') as f:
            domTree.writexml(f, encoding='UTF-8')
        LOG_DEBUG("Update {} success".format(file_name))
        return True
    except:
        LOG_DEBUG("Update {} fail".format(file_name))
        return False


def remove_targets(file_name, app_target_list):
    LOG_DEBUG("Start remove targets of {}".format(file_name))
    domTree = read_xml(file_name)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("Target")
    targets_parent_node = rootNode.getElementsByTagName("Targets")[0]

    for t in targets:
        target_name = t.childNodes[1].childNodes[0].data
        if target_name not in app_target_list:
            LOG_DEBUG("> remove {}".format(target_name))
            targets_parent_node.removeChild(t)
    write_xml(domTree, file_name)
    
    return app_target_list


def include_or_exclude_files(file_name, app_target_list, build_include_files, build_exclude_files):
    LOG_DEBUG("Start include and exclude project files {}".format(file_name))
    domTree = read_xml(file_name)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("Target")

    if not build_include_files and not build_exclude_files:
        LOG_DEBUG("Empty include and exclude file lists, skip")
        return True

    for target in targets:
        if target.childNodes[1].childNodes[0].data not in app_target_list:
            continue
        groups = target.getElementsByTagName("GroupName")
        for group in groups:
            group_name = group.childNodes[0].data
            if group_name in list(build_include_files.keys()) + list(build_exclude_files.keys()):
                files = group.parentNode.getElementsByTagName("FileName")
                for f in files:
                    f_name = f.childNodes[0].data
                    if f_name in build_exclude_files.get(group_name, []):
                        node = add_include_node(domTree, "FileOption", 0)
                        f.parentNode.appendChild(node)
                    if f_name in build_include_files.get(group_name, []):
                        node = add_include_node(domTree, "FileOption", 1)
                        f.parentNode.appendChild(node)
        
    return write_xml(domTree, file_name)


def include_or_exclude_groups(file_name, app_target_list, build_include_groups, build_exclude_groups):
    LOG_DEBUG("Start include and exclude project groups {}".format(file_name))
    domTree = read_xml(file_name)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("Target")

    if not build_include_groups and not build_exclude_groups:
        LOG_DEBUG("Empty include and exclude group lists, skip")
        return True

    for target in targets:
        if target.childNodes[1].childNodes[0].data not in app_target_list:
            continue
        groups = target.getElementsByTagName("GroupName")
        for group in groups:
            group_name = group.childNodes[0].data
            if group_name in build_exclude_groups:
                LOG_DEBUG("Exclude group {} for {}".format(group_name, target.childNodes[1].childNodes[0].data))
                node = add_include_node(domTree, "GroupOption", 0)
                group.parentNode.appendChild(node)
            if group_name in build_include_groups:
                LOG_DEBUG("Include group {} for {}".format(group_name, target.childNodes[1].childNodes[0].data))
                node = add_include_node(domTree, "GroupOption", 1)
                group.parentNode.appendChild(node)
    
    return write_xml(domTree, file_name)


def find_target_for_project(project_path, keyword=''):
    target_list = []
    tree = ET.parse(project_path)
    targetname_tag_list = tree.findall('Targets/Target/TargetName')
    for i in targetname_tag_list:
        if not keyword or (keyword and re.search(keyword, i.text)):
            target_list.append(i.text)
    LOG_DEBUG("{} target list with keyword {}: {}".format(project_path, keyword, target_list))   
    return list(set(target_list))


def all_group_files_exist(file_name, group_name, app_target):
    domTree = read_xml(file_name)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("Target")
    cwd = os.getcwd()
    os.chdir(os.path.dirname(file_name))
    enable_group = True
    miss_list = list()

    for target in targets:
        if target.childNodes[1].childNodes[0].data != app_target:
            continue
        groups = target.getElementsByTagName("GroupName")
        group_name_list = list(map(lambda g: g.childNodes[0].data, groups))
        if group_name not in group_name_list:
            LOG_DEBUG("{} doesn't exists, skip".format(group_name))
            enable_group = False
            break
        for group in groups:
            files = group.parentNode.getElementsByTagName("File")
            file_list = list(map(lambda l: l.getElementsByTagName('FilePath')[0].childNodes[0].data, files))
            if group.childNodes[0].data == 'lib':
                for lib in file_list:
                    if re.search(group_name, lib) and not os.path.exists(lib):
                        miss_list.append(lib)

            if group.childNodes[0].data == group_name:
                for f in file_list:
                    if not os.path.exists(f):
                        LOG_DEBUG("{} doesn't exists, skip enable group {}".format(f, group_name))
                        miss_list.append(f)

    enable_group = False if miss_list else enable_group
    LOG_DEBUG("Enable group {}: {}, miss list for {}: {}".format(group_name, enable_group, group_name, miss_list))
    os.chdir(cwd)

    return enable_group, miss_list


def change_project_option_defines(full_path, targets_need_to_change_list, macro, value):
    LOG_INFO("Start change project option defines: {}".format(full_path))
    domTree = read_xml(full_path)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("TargetName")
    
    for target in targets:
        if target.childNodes[0].data not in targets_need_to_change_list:
            continue

        definitions = target.parentNode.getElementsByTagName("Define")
        if definitions[0].firstChild:
            res = re.search(r'.*({}\s*=\s*\d+).*'.format(macro), definitions[0].childNodes[0].data)
            if value == 'remove':
                if res:
                    definitions[0].childNodes[0].data = re.sub(res.group(1), r'', definitions[0].childNodes[0].data)
                    LOG_INFO("********Macro exists, delete {} for {}********".format(res.group(1), target.childNodes[0].data))
                elif '{},'.format(macro) in definitions[0].childNodes[0].data:
                    definitions[0].childNodes[0].data = definitions[0].childNodes[0].data.replace('{},'.format(macro), '')
                    LOG_INFO("********Macro exists, delete {}, for {}********".format(macro, target.childNodes[0].data))
            else:
                if res:
                    definitions[0].childNodes[0].data = re.sub(res.group(1), r'{}={}'.format(macro, value), definitions[0].childNodes[0].data)
                    LOG_INFO("********Macro exists, set {}={} for {}********".format(macro, value, target.childNodes[0].data))
                else:
                    definitions[0].childNodes[0].data += r' {}={}'.format(macro, value)
                    LOG_INFO("********Macro doesn't exist, add {}={} for {}********".format(macro, value, target.childNodes[0].data))
        else:
            if value == 'remove':
                LOG_INFO("********Empty definition, skip********")
                continue
            else:
                item = domTree.createTextNode("{} = {}".format(macro, value))
                definitions[0].appendChild(item)
                LOG_INFO("********Empty definition, add {}={} for {}********".format(macro, value, target.childNodes[0].data))

    write_xml(domTree, full_path)


def update_macro(file_path, key, value, count=0, value_pattern=None):
    with open(file_path, mode='r', newline='', errors='surrogateescape') as fd:
        stream = fd.read()
        value_pattern = value_pattern or r"\d+"
        if re.search(r"\s*#define\s+{}\s+".format(key), stream):
            LOG_DEBUG("Find {} macro definition, start replace macro value".format(key))
            stream = re.sub(r'^(\s*#define\s+{}\s+)({})'.format(key, value_pattern), lambda objs:objs.group(1)+value, stream, count=count, flags=re.M)
        else:
            stream += "\n#undef {}\n#define {}\t{}\n".format(key, key, value)

    with open(file_path, mode='w+', newline='', errors='surrogateescape') as fd:
        fd.write(stream)
    #LOG_DEBUG('Configure {} diff ->\n{}'.format(os.path.basename(file_path), repo.git.diff(file_path)))


def update_overlay_func(file_path, func_name, func_string_info):
    func_start_index = -1
    func_end_index = -1
    with open(file_path, "r", encoding="utf-8") as f:
        lines = f.readlines()
    for i, line in enumerate(lines):
        if line.startswith("&{}".format(func_name)):
            func_start_index = i
            continue
        if func_start_index != -1 and line.startswith("};"):
            func_end_index = i
            break

    if func_start_index != -1 and func_end_index != -1:
        for k, v in func_string_info:
            for i in range(func_start_index, func_end_index + 1):
                if k in lines[i]:
                    lines[i] = lines[i].replace(k, v)

    with open(file_path, "w", encoding="utf-8") as f:
        f.writelines(lines)


def update_func_string(file_path, func_name, func_string_info):
    import CppHeaderParser
    cppheader = CppHeaderParser.CppHeader(file_path)
    all_items = cppheader.functions
    all_items = sorted(all_items, key=lambda x: x['line_number'])

    if func_name not in list(map(lambda x: x["name"], all_items)):
        LOG_DEBUG("Function {} doesn't exist, please check function name".format(func_name))
        return

    for i in range(len(all_items)):
        if i + 1 == len(all_items):
            all_items[i]["last_line_number"] = None
        else:
            all_items[i]["last_line_number"] = all_items[i+1]['line_number'] - 1

    with open(file_path, encoding='utf-8', errors='surrogateescape', mode='r', newline='') as fd:
        all_file_lines = fd.readlines()
    all_file_lines = [None] + all_file_lines
    
    for item in all_items:
        if item["name"] == func_name:
            if item["last_line_number"]:
                last_line_number = item["last_line_number"]
            else:
                last_line_number = len(all_file_lines) - 1
            all_file_lines[item["line_number"]] = "".join(all_file_lines[item["line_number"] : last_line_number + 1])
            for i in range(item["line_number"] + 1, last_line_number + 1, 1):
                all_file_lines[i] = None
            
            # change file content here
            all_file_lines[item["line_number"]] = modify_func_string(item['name'], all_file_lines[item["line_number"]], func_string_info)

    all_file_lines = list(filter(lambda x: x, all_file_lines))
    with open(file_path, encoding='utf-8', errors='surrogateescape', mode='w', newline='') as fd:
        fd.writelines(all_file_lines)
    LOG_DEBUG("Update {}".format(file_path))

def remove_comments(src_string):
    ''' Remove c-style/cpp-style comments and blank lines'''
    regex1 = re.compile(r'''/\*[^*]*\*+([^/*][^*]*\*+)*/''', re.MULTILINE|re.DOTALL)
    regex2 = re.compile(r'//.*$', re.MULTILINE)
    dest_string = regex1.sub(lambda obj: ' '* len(obj.group(0)), src_string)
    dest_string = regex2.sub(lambda obj: ' '* len(obj.group(0)), dest_string)
    return dest_string

def get_func_start_end_pos_in_func_string(func_name, func_string):
    '''
    return (index("{"), index("}"))
    '''
    func_string = remove_comments(func_string)
    search_start = func_string.index("{") - 1
    search_end = len(func_string)

    bracket_count = 0
    fun_end_pos = search_start
    for i in range(search_start, search_end):
        if func_string[i] == r'{':
            bracket_count = bracket_count + 1
            continue
        if func_string[i] == r'}':
            bracket_count = bracket_count - 1
            if(bracket_count == 0):
                fun_end_pos = i
                break
    if fun_end_pos == search_start:
        sys.exit("Can not find content of {}".format(func_name))

    return (search_start + 1, fun_end_pos)

def modify_func_string(func_name, func_string, func_string_info):
    start_pos, end_pos = get_func_start_end_pos_in_func_string(func_name, func_string)
    content_string = func_string[start_pos + 1 : end_pos].split("\n")
    for s, d in func_string_info:
        for i, line in enumerate(content_string):
            content_string[i] = re.sub(s, d, line) if re.search(s, line) else line

    return func_string[0 : start_pos + 1] + "\n".join(content_string) + func_string[end_pos : ]



def remove_before_make_for_sdk(project, app_target_list):
    LOG_DEBUG("Start remove before make for sdk: {}".format(project))
    domTree = read_xml(project)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("TargetName")
    
    for target in targets:
        if target.childNodes[0].data not in app_target_list:
            continue
        beforemake = target.parentNode.getElementsByTagName("BeforeMake")[0]
        version_gen = beforemake.getElementsByTagName("RunUserProg1")[0]
        version_gen.childNodes[0].data = "0"

    write_xml(domTree, project)


def add_include_path(project, include_path, app_target_list):
    print("Start add include path {} of {}".format(include_path, project))
    domTree = read_xml(project)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("TargetName")
    
    for target in targets:
        target_name = target.childNodes[0].data
        if target.childNodes[0].data not in app_target_list:
            continue
        target_options = target.parentNode.getElementsByTagName("TargetOption")
        for option in target_options:
            arm_ads_nodes = option.getElementsByTagName("TargetArmAds")
            for arm_ads in arm_ads_nodes:
                cads_nodes = arm_ads.getElementsByTagName("Cads")
                for cads in cads_nodes:
                    include_paths = cads.getElementsByTagName("IncludePath")
                    if include_paths:
                        inc_path = include_paths[0]
                        current_paths = inc_path.firstChild.data if inc_path.firstChild else ""
                        
                        if current_paths:
                            paths_list = current_paths.split(';')
                            if include_path not in paths_list:
                                updated_paths = current_paths + ';' + include_path
                                inc_path.firstChild.data = updated_paths
                                print(f"Updated IncludePath for target '{target_name}'")
                        else:
                            text_node = domTree.createTextNode(include_path)
                            inc_path.appendChild(text_node)
                            print(f"Set new IncludePath for target '{target_name}'")

    return write_xml(domTree, project)


def create_file_node(domTree, file_name, file_type, file_path):
    """Create a new File node with given details."""
    file_node = domTree.createElement('File')
    
    file_name_node = domTree.createElement('FileName')
    file_name_node.appendChild(domTree.createTextNode(file_name))
    
    file_type_node = domTree.createElement('FileType')
    file_type_node.appendChild(domTree.createTextNode(file_type))
    
    file_path_node = domTree.createElement('FilePath')
    file_path_node.appendChild(domTree.createTextNode(file_path))
    
    file_node.appendChild(file_name_node)
    file_node.appendChild(file_type_node)
    file_node.appendChild(file_path_node)
    
    return file_node


def add_include_files(project, include_files, app_target_list):
    print("Start add include files {} of {}".format(include_files, project))
    domTree = read_xml(project)
    rootNode = domTree.documentElement
    targets = rootNode.getElementsByTagName("Target")

    for target in targets:
        if target.childNodes[1].childNodes[0].data not in app_target_list:
            continue
        groups = target.getElementsByTagName("GroupName")
        group_name_list = list(map(lambda g: g.childNodes[0].data, groups))
        for include_file, file_type, group_name in include_files:
            file_name = os.path.basename(include_file)
            if group_name not in group_name_list:
                LOG_DEBUG("{} doesn't exists, skip".format(group_name))
                continue
            for group in groups:
                if group.childNodes[0].data == group_name:
                    files_node = group.parentNode.getElementsByTagName("Files")[0]
                    file_list = [file.getElementsByTagName('FileName')[0].childNodes[0].data 
                                 for file in files_node.getElementsByTagName('File')]
                    if file_name in file_list:
                        print(f"{file_name} already exists in {group_name}, skip")
                        continue
                    # Create new File node and append it to Files
                    new_file_node = create_file_node(domTree, file_name, file_type, include_file)
                    files_node.appendChild(new_file_node)
                    print(f"Added {file_name} to {group_name}")
    
    return write_xml(domTree, project)