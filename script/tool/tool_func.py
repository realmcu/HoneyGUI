import re
import os
import sys
import time
import win32file
import win32pipe
from xml.dom.minidom import parse
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import log


def test_tool_communication(pip_name, log_path, log_tool, process_info, logtool_dict, log_zip_queue, test_type, test_module):
    if not pip_name:
        return
    PIPE_BUFFER_SIZE = 65535
    pip_name = r'\\.\pipe\{}'.format(pip_name)
    named_pipe = win32pipe.CreateNamedPipe(pip_name,
                                        win32pipe.PIPE_ACCESS_DUPLEX,
                                        win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_WAIT | win32pipe.PIPE_READMODE_MESSAGE,
                                        win32pipe.PIPE_UNLIMITED_INSTANCES,
                                        PIPE_BUFFER_SIZE,
                                        PIPE_BUFFER_SIZE,5, None)
    print("server started")
    while True:
        try:
            win32pipe.ConnectNamedPipe(named_pipe, None)
            print('Client connect')
            try:
                data = win32file.ReadFile(named_pipe, PIPE_BUFFER_SIZE, None)
                if data is None or len(data) < 2:
                    raise Exception("invalid data")
                receive_msg = data[1].decode('utf-8')
                if '[jenkins]test done' in receive_msg:
                    print("[jenkins]test done, finish and exit")
                    break
                elif '[HoneyRobotSuite]' in receive_msg or '[HoneyRobotLight]' in receive_msg:
                    tc_name = re.search(r"\[HoneyRobot.*\]\s*(.*)", receive_msg).group(1).strip()
                    print(receive_msg)
                    try:
                        time.sleep(5)
                        log.copy_log_and_restart_logtool(tc_name=tc_name, logtool_dict=logtool_dict, 
                                                        log_tool=log_tool, process_info=process_info,
                                                        log_path=log_path, test_type=test_type,
                                                        log_zip_queue=log_zip_queue, test_module=test_module)
                    except Exception as e:
                        print("copy_log_and_restart_logtool exception: {}".format(e), flush=True)
                    finally:
                        try:
                            win32file.WriteFile(named_pipe, "OK".encode('utf-8'))
                            win32file.FlushFileBuffers(named_pipe)
                            time.sleep(1)
                        except Exception as e:
                            print("Exception in write file {}".format(e))
                else:
                    print("ignore the message: {}".format(receive_msg), flush=True)
            except Exception as e:
                print("exception:", e)
        finally:
            try:
                win32pipe.DisconnectNamedPipe(named_pipe)
                print("Disconnect client", flush=True)
            except:
                pass
    win32file.CloseHandle(named_pipe)
    print("No need to wait hrs information anymore")


def generate_description_for_tc(tool_path, tc_name):
    script_path = os.path.abspath(os.path.join(os.path.dirname(tool_path), "../Scripts"))
    for dir_path, _, file_names in os.walk(script_path):
        for f in file_names:
            if f.lower() == tc_name.lower():
                f_path = os.path.abspath(os.path.join(dir_path, f))
                if f_path.endswith(".xml"):
                    return f_path
                f_description = r'..\Scripts' + f_path[len(script_path) : ]
                return f_description
    return None


def read_xml(file_name):
    if not os.path.exists(file_name):
        sys.exit("File {} doesn't exists".format(file_name))
    domTree = parse(file_name)

    return domTree


def write_xml(domTree, file_name):
    try:
        with open(file_name, 'w', newline='') as f:
            domTree.writexml(f, addindent='  ', encoding='utf-8')
        print("Write {} success".format(file_name))
        return True
    except:
        print("Write {} fail".format(file_name))
        return False


def set_element_value(rootNode, element, value):
    node = rootNode.getElementsByTagName(element)[0]
    node.childNodes[0].data = value


def reset_dial_case_loop_number(case_list, loop_number):
    for case in case_list:
        if "dial" in case.lower():
            print("{} is dial case, reset case loop number".format(case))
            loop_number = 15 if int(loop_number) > 15 else loop_number
            print("loop number set as {}".format(loop_number))
            return loop_number
    print("No need to reset loop number")
    return loop_number


def generate_xml_for_freeStyle(test_module, tool_path, orig_sequence_path):
    print("generate_xml_for_freeStyle")

    #get all test cases need to test
    tc_need_to_test = list()
    loop_number = 0
    max_fail_number = 0
    xml_folder = os.path.join(os.getenv("WORKSPACE"), 'script')
    new_xml = os.path.join(xml_folder,'{}.xml'.format(test_module))
    with open(os.path.join(xml_folder,'{}_raw.stb'.format(test_module)), mode='r', newline='', errors='surrogateescape') as fd:
        lines = fd.readlines()
        loop_number = re.search(r"cfg_LoopNumber\t(\d+)", lines[0]).group(1)
        max_fail_number = re.search(r"cfg_maxFailNumber\t(\d+)", lines[1]).group(1)
        for i in range(2, len(lines)):
            line_content = os.path.splitext(lines[i].lstrip().strip('\n').strip())[0]
            if line_content:
                tc_need_to_test.append(line_content)
    loop_number = reset_dial_case_loop_number(tc_need_to_test, loop_number)
    domTree = read_xml(orig_sequence_path)
    rootNode = domTree.documentElement
    #update global paramters
    if int(loop_number) != 0:
        set_element_value(rootNode, "cfg_LoopNumber", loop_number)
    if int(max_fail_number) != 0:
        set_element_value(rootNode, "cfg_maxFailNumber", max_fail_number)
    actions = rootNode.getElementsByTagName("action")
    #delete useless actions
    for action in actions:
        action.parentNode.removeChild(action)
    for tc in tc_need_to_test:
        tc_xml = generate_description_for_tc(tool_path, tc + r'.xml')
        if tc_xml:
            print("TestCase is xml")
            return tc_xml
        tc_road = generate_description_for_tc(tool_path, tc + r'.stsx')
        if not tc_road:
            print("Can not find tc {}".format(tc))
            return new_xml
        action_node = domTree.createElement("action")
        action_node.setAttribute("title", os.path.basename(tc))
        action_node.setAttribute("description", "")
        road_node = domTree.createElement("road")
        road_node.setAttribute("description", tc_road)
        action_node.appendChild(road_node)
        rootNode.appendChild(action_node)
    write_xml(domTree, new_xml)

    return new_xml