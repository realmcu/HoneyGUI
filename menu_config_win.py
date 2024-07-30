import tkinter as tk
from tkinter import messagebox, scrolledtext
import re
import subprocess
import os
import sys
import atexit

def get_script_dir():
    if getattr(sys, 'frozen', False):
        return os.path.dirname(sys.executable)
    
    return os.path.dirname(os.path.abspath(__file__))

def load_config_file(file_path):
    config_sections = []  # To hold sections with primary and secondary options
    primary_options = []  # To hold primary options found in the second line of <e> blocks

    with open(file_path, "r") as file:
        config_lines = file.readlines()

    section = None
    for index, line in enumerate(config_lines):
        line = line.strip()

        if re.match(r"^//\s*<e>", line):
            next_index = index + 1
            if next_index < len(config_lines):
                next_line = config_lines[next_index].strip()
                match = re.match(r"^(//\s*)?#define\s+(CONFIG_REALTEK_BUILD_\w+)\s*(.*?)$", next_line)
            if match:
                primary_name = match.group(2)
                primary_value = match.group(3).strip()
                is_commented = bool(match.group(1))
                is_enabled = False if is_commented else (primary_value == '1')

                primary_options.append({'name': primary_name, 'value': is_enabled})
                section = {
                    'primary': primary_name,
                    'secondary_single': [],
                    'secondary_multi': [],
                    'start_index': index
                }

        if section and re.match(r"^//\s*<h>\s*HoneyGUI Config Function\s*", line):
            for sub_index in range(index + 1, len(config_lines)):
                sub_line = config_lines[sub_index].strip()
                match = re.match(r"^(//\s*)?#define\s+(CONFIG_REALTEK_BUILD_\w+)\s*(.*?)$", sub_line)
                if match:
                    secondary_name = match.group(2)
                    secondary_value = match.group(3).strip()
                    is_commented = bool(match.group(1))
                    is_enabled = not is_commented if secondary_value == '' else secondary_value == '1'

                    secondary_option = {
                        'name': secondary_name,
                        'value': is_enabled
                    }
                    section['secondary_multi'].append(secondary_option)
                if re.match(r"^//\s*</h>", sub_line):
                    break

        elif section and re.match(r"^//\s*<h>", line):
            for sub_index in range(index + 1, len(config_lines)):
                sub_line = config_lines[sub_index].strip()
                match = re.match(r"^(//\s*)?#define\s+(CONFIG_REALTEK_BUILD_\w+)\s*(.*?)$", sub_line)
                if match:
                    secondary_name = match.group(2)
                    secondary_value = match.group(3).strip()
                    is_commented = bool(match.group(1))
                    is_enabled = not is_commented if secondary_value == '' else secondary_value == '1'

                    secondary_option = {
                        'name': secondary_name,
                        'value': is_enabled
                    }
                    section['secondary_single'].append(secondary_option)

                if re.match(r"^//\s*</h>", sub_line):
                    break

        elif section and re.match(r"^//\s*</e>", line):
            config_sections.append(section)
            section = None

    return primary_options, config_sections

class ConfigEditorApp:
    def __init__(self, master, file_path):
        self.master = master
        self.master.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.info_window = None
        self.scons_process = None
        self.gui_process = None
        atexit.register(self.cleanup)

        self.file_path = file_path
        self.script_dir = get_script_dir()
        self.primary_options, self.config_sections = load_config_file(file_path)
        
        # Initialize the StringVar for primary options with the default checked value
        # self.primary_group = tk.StringVar(value=self.get_default_primary())
        self.primary_groups = {}
        self.secondary_groups = {}
        self.secondary_vars = {}

        self.create_widgets()

    def on_closing(self):
        self.master.destroy()     
        
    def get_default_primary(self):
        for primary_option in self.primary_options:
            if primary_option['value']:
                return primary_option['name']
        return None  # None if no primary option is selected
    
    def get_default_secondary(self, secondary_options):
        for sec_option in secondary_options:
            if sec_option['value']:
                return sec_option['name']
        return None  # None if no secondary option is selected
        
    def run_command(self):
        win32_sim_path = os.path.join(self.script_dir, "win32_sim")
        with open(self.file_path, "r") as file:
            config_lines = file.readlines()

        with open(self.file_path, "w") as file:
            for line in config_lines:
                stripped_line = line.strip()
                match = re.match(r"^(//\s*)?#define\s+(CONFIG_REALTEK_BUILD_\w+)(.*)", stripped_line)
                if match:
                    option = re.findall(r"#define\s+(CONFIG_REALTEK_BUILD_\w+)", stripped_line)[0]
                    remaining_part = match.group(3) if match.group(3) else ""
                    should_enable = False
                    
                    # Check for primary options
                    for primary_option in self.primary_options:
                        if primary_option['name'] == option:
                            if primary_option['variable'].get():
                                should_enable = True
                                print(should_enable)
                            

                    # Check for secondary options
                    for section in self.config_sections:
                        for sec in section['secondary_single']:
                            if sec['name'] == option:
                                if self.secondary_groups[section['primary']].get() == option:
                                    should_enable = True

                        for sec in section['secondary_multi']:
                            if sec['name'] == option and self.secondary_vars[sec['name']].get():
                                should_enable = True

                    if should_enable:
                        if option == "CONFIG_REALTEK_BUILD_GUI" or option == "CONFIG_REALTEK_BUILD_LVGL_GUI" or option == "CONFIG_REALTEK_BUILD_ARM_2D":
                            file.write("#define {}{}\n".format(option, "     1"))
                        else:
                            file.write("#define {}{}\n".format(option, remaining_part))
                    else:
                        if option == "CONFIG_REALTEK_BUILD_GUI" or option == "CONFIG_REALTEK_BUILD_LVGL_GUI" or option == "CONFIG_REALTEK_BUILD_ARM_2D":
                            file.write("#define {}{}\n".format(option, "     0"))
                        else:
                            file.write("// #define {}{}\n".format(option, remaining_part))
                else:
                    file.write(line)

        self.run_scons(win32_sim_path)

    def safe_winfo_exists(self, window):
        try:
            return window.winfo_exists()
        except tk.TclError:
            return False

    def terminate_gui_process(self):
        if self.gui_process is not None:
            if self.gui_process.poll() is None:
                self.gui_process.terminate()
                try:
                    self.gui_process.wait(timeout=5)
                    print("Previous gui_process terminated.")
                except subprocess.TimeoutExpired:
                    print("gui_process did not terminate in time. Forcing termination.")
                    self.gui_process.kill()


    def run_scons(self, win32_sim_path):
        if self.info_window is not None and self.safe_winfo_exists(self.info_window):
                self.info_window.destroy()

        self.info_window = tk.Toplevel(self.master)
        self.info_window.title("Scons Information")
        self.info_window.geometry("600x400")
        
        self.text_box = scrolledtext.ScrolledText(self.info_window, wrap="word")
        self.text_box.pack(fill="both", expand=True)
        self.text_box.config(state="disabled")

        self.terminate_gui_process()

        try:
            self.scons_process = subprocess.Popen(["scons"], cwd=win32_sim_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            self.read_process_output(self.scons_process)
        except Exception as e:
            messagebox.showerror("错误", f"构建失败:\n{str(e)}")
            
    def read_process_output(self, process):
        def read_line():
            line = process.stdout.readline()
            if line:
                self.text_box.config(state="normal")
                self.text_box.insert("end", line)
                self.text_box.config(state="disabled")
                self.text_box.yview("end")
                self.text_box.update_idletasks()
                self.master.after(10, read_line)
            else:
                return_code = process.poll()
                if return_code is None:
                    self.master.after(100, read_line)
                else:
                    process.stdout.close()
                    stderr_output = process.stderr.read()
                    process.stderr.close()
                    if return_code == 0:
                        self.run_gui_exe()
                    else:
                        self.show_error_window(return_code, stderr_output)
              
        read_line()

    def show_error_window(self, return_code, error_message):
        messagebox.showerror("错误", f"SCons 构建失败，退出代码: {return_code}\n{error_message}")

    def run_gui_exe(self):
        try:
            self.terminate_gui_process()

            gui_exe_path = os.path.join(self.script_dir, "win32_sim", "gui.exe")
            if not os.path.exists(gui_exe_path):
                raise FileNotFoundError(f"gui.exe 未找到在 '{gui_exe_path}'")
            self.gui_process = subprocess.Popen([gui_exe_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            if self.gui_process.poll() is None:
                print(f"gui.exe started successfully with PID: {self.gui_process.pid}")
            else:
                print("Failed to start gui.exe.")

        except Exception as e:
            messagebox.showerror("错误", f"GUI 启动失败:\n{str(e)}")        

    def create_widgets(self):
        self.master.grid_rowconfigure(0, weight=1)
        self.master.grid_columnconfigure(0, weight=1)

        top_frame = tk.Frame(self.master)
        top_frame.grid(row=0, column=0, columnspan=2, sticky='ew')

        left_space = tk.Frame(top_frame)
        left_space.pack(side=tk.LEFT, padx=20, expand=True, fill=tk.X)

        scons_run_button = tk.Button(top_frame, text="scons and run", command=self.run_command)
        scons_run_button.pack(side=tk.LEFT, padx=10)

        run_button = tk.Button(top_frame, text="run", command=self.run_gui_exe)
        run_button.pack(side=tk.LEFT, padx=10)

        right_space = tk.Frame(top_frame)
        right_space.pack(side=tk.LEFT, padx=20, expand=True, fill=tk.X)

        canvas = tk.Canvas(self.master)
        scrollbar = tk.Scrollbar(self.master, orient="vertical", command=canvas.yview)
        scrollable_frame = tk.Frame(canvas)

        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(
                scrollregion=canvas.bbox("all")
            )
        )

        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.grid(row=1, column=0, sticky='nsew')
        scrollbar.grid(row=1, column=1, sticky='ns')

        row_index = 0

        for primary_option in self.primary_options:
            # # Create a radiobutton for each primary option
            # primary_radio = tk.Radiobutton(
            #     scrollable_frame, text=primary_option['name'],
            #     variable=self.primary_group, value=primary_option['name']
            # )
            # primary_radio.grid(row=row_index, column=0, sticky='w')
            # row_index += 1

            var = tk.BooleanVar(value=primary_option['value'])
            primary_option['variable'] = var
            self.primary_groups[primary_option['name']] = var
            primary_check = tk.Checkbutton(scrollable_frame, text=primary_option['name'], variable=var)
            primary_check.grid(row=row_index, column=0, sticky='w')
            row_index += 1

            # Find corresponding secondary options
            for section in self.config_sections:
                if section['primary'] == primary_option['name']:
                    if section['secondary_single']:
                        secondary_group = tk.StringVar(value=self.get_default_secondary(section['secondary_single']))
                        self.secondary_groups[primary_option['name']] = secondary_group
                        for sec in section['secondary_single']:
                            radio_btn = tk.Radiobutton(scrollable_frame, text=sec['name'], variable=secondary_group, value=sec['name'])
                            radio_btn.grid(row=row_index, column=0, sticky='w', padx=20)
                            row_index += 1

                    if section['secondary_multi']:
                        for sec in section['secondary_multi']:
                            var = tk.BooleanVar(value=sec['value'])
                            sec['variable'] = var  # store variable in secondary option
                            self.secondary_vars[sec['name']] = var
                            chk = tk.Checkbutton(scrollable_frame, text=sec['name'], variable=var)
                            chk.grid(row=row_index, column=0, sticky='w', padx=20)
                            row_index += 1

    def cleanup(self):
        if self.scons_process is not None and self.scons_process.poll() is None:
            self.scons_process.terminate()
        if self.gui_process is not None and self.gui_process.poll() is None:
            self.gui_process.terminate()
        if self.info_window is not None and self.safe_winfo_exists(self.info_window):
            self.info_window.destroy()


if __name__ == "__main__":
    script_dir = get_script_dir()
    file_path = os.path.join(script_dir, "win32_sim", "menu_config.h")
    print("Loading File: ", file_path)

    root = tk.Tk()
    root.title("Menu Config")

    # Make the main window resizable on the x and y axes
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)

    app = ConfigEditorApp(root, file_path)

    root.mainloop()