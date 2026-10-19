import os
import sys
import time
import platform
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from test.error_record import record_error
from test.slave_property import slave_property_parse
from base.tool_base import ToolBase
from bee1control import bee1Controller

class Bee1Controller(bee1Controller):
    def __init__(self, project, branch, chip_type, role, module, part_number):
        self.project = project
        self.branch = branch
        self.chip_type = chip_type
        self.role = role
        self.module = module
        self.part_number = part_number
        self.serial_info = slave_property_parse(self.project, self.branch, self.chip_type, 'BEE1_CONTROLLER', 'role', role, part_number=part_number)

    def bee1_disable_all_controller_GPIO_pins(self):
        #return True
        if self.serial_info:
            bee_chip = self.serial_info.get("Chip", None) or "BEE1"
            serial_name = self.serial_info["COM"]
            if not isinstance(serial_name, list):
                serial_name = [serial_name]
            for t_serial in serial_name:
                t_bee1Controller = bee1Controller(serial_name = t_serial, Chip=bee_chip)
                res = t_bee1Controller.disable_all_pins()
                if not res:
                    record_error("BEEX Error: bee1_disable_all_controller_GPIO_pins: disable all pin with com {} failed".format(t_serial), need_exit=True)
                print("bee1_disable_all_controller_GPIO_pins: disable all pin with com {} sussess".format(t_serial))
        else:
            print("bee1_disable_all_controller_GPIO_pins: can not find bee1 control with role {}".format(self.role))


    def bee1_power_on_or_factory_reset(self, action_type, key_presstime, interval=0):
        #return True
        serial_name = self.serial_info["COM"]
        power_on_pin = self.serial_info["power_on"]
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip)
        print("==> [bee1_power_on_or_factory_reset] {} start: {}".format(action_type, time.strftime("%y_%m_%d_%H_%M_%S", time.localtime())))
        if action_type in ["power_on", "power_off", "factory_reset"]:
            res = t_bee1Controller.send_command(mode = 'KEY_PRESS', pins = power_on_pin, keypresstime = int(key_presstime*1000), close_serial = False)
            if not res:
                record_error("BEEX Error")
            time.sleep(key_presstime)
        else:
            print("bee1_power_on_or_factory_reset: {} is not supported.".format(action_type))
            res = False
        print("==> [bee1_power_on_or_factory_reset] {} finish: {}".format(action_type, time.strftime("%y_%m_%d_%H_%M_%S", time.localtime())))
        if not res:
            sys.exit(1)

        res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = power_on_pin, keypresstime = 1000, close_serial = True)
        if not res:
            record_error("BEEX Error")
        time.sleep(interval)
        if not res:
            sys.exit(1)


    def bee1_set_pin_mode(self, interval=0):
        select_pin = self.serial_info["select"]
        pin_mode = self.serial_info["mode"]
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        serial_name = self.serial_info["COM"]
        t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip)
        res = t_bee1Controller.send_command(mode = pin_mode, pins = select_pin)
        if not res:
            record_error("BEEX Error", need_exit=True)
        time.sleep(interval)


    def bee1_set_pins_for_usb(self, interval=0, mode = "HIGH"):
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        serial_name = self.serial_info["COM"]
        t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip)
        if mode == "HIGH":
            vdap_pin = self.serial_info["vdap_pin"]
            reset_pin = self.serial_info["reset_pin"]
            res = t_bee1Controller.send_command(mode = 'HIGH', pins = vdap_pin)
            if not res:
                record_error("BEEX Error", need_exit=True)
            time.sleep(2)
            res = t_bee1Controller.send_command(mode = 'KEY_PRESS', pins = reset_pin)
            if not res:
                record_error("BEEX Error", need_exit=True)
            res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = reset_pin)
            if not res:
                record_error("BEEX Error", need_exit=True)
        elif mode == "KEY_PRESS":
            reset_controller_pin = self.serial_info["reset_controller_pin"]
            res = t_bee1Controller.send_command(mode = 'KEY_PRESS', pins = reset_controller_pin)
            if not res:
                record_error("BEEX Error", need_exit=True)
            res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = reset_controller_pin)
            if not res:
                record_error("BEEX Error", need_exit=True)
        time.sleep(interval)


    def bee1_set_pins_for_power(self, interval=0):
        select_pin = self.serial_info["select"]
        unselect_pins = list(filter(lambda x: "unselect" in x, self.serial_info.keys()))
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        serial_name = self.serial_info["COM"]
        t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip)
        for p in unselect_pins:
            res = t_bee1Controller.send_command(mode = 'HIGH', pins = self.serial_info[p])
            if not res:
                 record_error("BEEX Error", need_exit=True)
        res = t_bee1Controller.send_command(mode = 'LOW', pins = select_pin)
        if not res:
            record_error("BEEX Error", need_exit=True)
        time.sleep(interval)


    def bee1_restart(self, interval=0):
        #return True
        serial_name = self.serial_info["COM"]
        reset_pin = self.serial_info["reset_pin"]
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip) 
        res = t_bee1Controller.send_command(mode = 'KEY_PRESS', pins = reset_pin, keypresstime = 1000, close_serial = False)
        if not res:
            record_error("BEEX Error", need_exit=True)
        time.sleep(1)
        res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = reset_pin, keypresstime = 1000, close_serial = True)
        if not res:
            record_error("BEEX Error", need_exit=True)
        time.sleep(interval)


    def bee1_reset(self, reset_type, interval=0):
        #return True
        serial_name = self.serial_info["COM"]
        reset_pin = self.serial_info["reset_pin"]
        P2_0 = self.serial_info["P2_0"]
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        if reset_type == 'reset_pre':
            t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip) 
            res = t_bee1Controller.send_command(mode = 'LOW', pins = P2_0, close_serial = False)
            if not res:
                sys.exit(1)
            res = t_bee1Controller.send_command(mode = 'KEY_PRESS', pins = reset_pin, keypresstime = 1000, close_serial = False)
            time.sleep(1)
            if not res:
                record_error("BEEX Error", need_exit=True)
            res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = reset_pin, close_serial = True)
            if not res:
                record_error("BEEX Error", need_exit=True)
        elif reset_type == 'reset_after':
            t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip) 
            res = t_bee1Controller.send_command(mode = 'HIGH', pins = P2_0, close_serial = False)
            if not res:
                record_error("BEEX Error", need_exit=True)
            res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = P2_0, close_serial = True)
            if not res:
                record_error("BEEX Error", need_exit=True)
        else:
            sys.exit("bee1_reset reset_type {} is not supported.".format(reset_type))
        time.sleep(interval)


    def bee1_key_press(self, key_name, interval=0):
        serial_name = self.serial_info["COM"]
        reset_pin = self.serial_info[key_name]
        bee_chip = self.serial_info.get("Chip", None) or "BEE1"
        t_bee1Controller = bee1Controller(serial_name = serial_name, Chip=bee_chip)
        res = t_bee1Controller.send_command(mode = 'KEY_PRESS', pins = reset_pin, keypresstime = 1000, close_serial = False)
        if not res:
            record_error("BEEX Error", need_exit=True)
        time.sleep(1)
        res = t_bee1Controller.send_command(mode = 'KEY_DISABLE', pins = reset_pin, keypresstime = 1000, close_serial = True)
        if not res:
            record_error("BEEX Error", need_exit=True)
        time.sleep(interval)
    
    
    def test_cut_disable_all_controller_GPIO_pins(self):
        if self.serial_info:
            print("Use bee1 to control the cut reset")
            self.bee1_disable_all_controller_GPIO_pins()
        else:
            print("Only bee1 need to disable all GPIO")


    def test_cut_reset(self, reset_type, bcut=None, interval=0):
        if self.serial_info:
            print("Use bee1 to control the cut reset: {}".format(reset_type))
            self.bee1_reset(reset_type=reset_type, interval=interval)
        else:
            print("Use b cut to control the cut reset: {}".format(reset_type))
            platform_unme = platform.uname()
            if 'Linux' in platform_unme and bcut:
                bcut.bcut_reset(reset_type = reset_type, interval=interval)
            else:
                self.honeyrobotsuite_reset(reset_type = reset_type, interval=interval)


    def test_cut_restart(self, bcut=None, interval=0):
        if self.serial_info:
            print("Use bee1 to control the cut restart")
            self.bee1_restart(interval=interval)
        else:
            print("Use b cut to control the cut restart")
            platform_unme = platform.uname()
            if 'Linux' in platform_unme and bcut:
                bcut.bcut_restart(interval=interval)
            else:
                self.honeyrobotsuite_restart(interval=interval)


    def test_cut_power_on_or_factory_reset(self, action_type, keypress_time, bcut=None, interval=0):
        if self.serial_info:
            print("Use bee1 to control the cut power on and factory reset")
            self.bee1_power_on_or_factory_reset(action_type = action_type, key_presstime=keypress_time, interval=interval)
        else:
            print("Use b cut to control the cut power on and factory reset")
            platform_unme = platform.uname()
            if 'Linux' in platform_unme and bcut:
                bcut.bcut_power_on_or_factory_reset(action_type=action_type, keypress_time=keypress_time, interval=1)
            else:
                self.honeyrobotsuite_power_on_or_factory_reset(action_type=action_type, interval=1)


    def honeyrobotsuite_power_on_or_factory_reset(self, action_type, interval=0):
        tool = slave_property_parse(self.project, self.branch, self.chip_type, 'HoneyRobotSuite', 'role', self.role, part_number=self.part_number)
        tool_path = tool['path']
        tool_devices = tool['devices']
        if action_type == "power_on":
            tool_action = tool['power_on']
        elif action_type == "factory_reset":
            tool_action = tool['factory_reset']
        elif action_type == "power_off":
            tool_action = tool['power_off']
        else:
            sys.exit("honeyrobotsuite_power_on_or_factory_reset: {} is not supported.".format(action_type))
        cmd = [tool_path, '-j0',
            '-devices', tool_devices,
            '-userCmd', tool_action]
        ToolBase.tool_run(cmd, delay=interval)


    def honeyrobotsuite_reset(self, reset_type, interval=0):
        '''Reset testing device by HoneyRobotSuite.'''
        tool = slave_property_parse(self.project, self.branch, self.chip_type, 'HoneyRobotSuite', 'role', self.role, part_number=self.part_number)
        tool_path = tool['path']
        tool_devices = tool['devices']
        tool_action = tool[reset_type]
        cmd = [tool_path, '-j0',
            '-devices', tool_devices,
            '-userCmd', tool_action]
        ToolBase.tool_run(cmd, delay=interval)


    def honeyrobotsuite_restart(self, interval=0):
        '''Resart testing device by HoneyRobotSuite.'''
        tool = slave_property_parse(self.project, self.branch, self.chip_type, 'HoneyRobotSuite', 'role', self.role, part_number=self.part_number)
        tool_path = tool['path']
        tool_devices = tool['devices']
        tool_action = tool['restart']
        cmd = [tool_path, '-j0',
            '-devices', tool_devices,
            '-userCmd', tool_action]
        ToolBase.tool_run(cmd, delay=interval)