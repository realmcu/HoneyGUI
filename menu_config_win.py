import tkinter as tk
from tkinter import messagebox, scrolledtext
import re
import subprocess
import os
import sys
import atexit
import threading
import queue

def get_script_dir():
    if getattr(sys, 'frozen', False):
        return os.path.dirname(sys.executable)
    
    return os.path.dirname(os.path.abspath(__file__))

def load_config_file(file_path):
    primary_options = [
        {"name": key, **info} 
        for key, info in products_info.items()
    ]
    return primary_options


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
        self.primary_options = load_config_file(file_path)
        
        # Initialize the StringVar for primary options with the default checked value
        self.primary_var = tk.StringVar(value=self.get_default_primary())

        self.create_widgets()

    def on_closing(self):
        self.master.destroy()     
        
    def get_default_primary(self):
        try:
            with open(self.file_path, "r") as file:
                config_lines = file.readlines()
            for option in self.primary_options:
                for line in config_lines:
                    if re.match(rf"^#define\s+{option['macro']}\b", line.strip()):
                        
                        return option['name']

        except Exception as e:
            print(f"Error reading config file: {e}")
        return None
        
    def run_command(self):
        win32_sim_path = os.path.join(self.script_dir, "win32_sim")
        with open(self.file_path, "r") as file:
            config_lines = file.readlines()

        selected_option = self.primary_var.get()

        with open(self.file_path, "w") as file:
            for line in config_lines:
                new_line = line

                for option in self.primary_options:
                    if re.search(rf"^//\s*#define\s+{option['macro']}\b|^#define\s+{option['macro']}\b", line.strip()):
                        if option['name'] == selected_option:
                            new_line = f"#define {option['macro']}\n"
                        else:
                            new_line = f"// #define {option['macro']}\n"
                        break

                file.write(new_line)
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

            self.output_queue = queue.Queue()
            threading.Thread(target=self.read_process_output, args=(self.scons_process.stdout, "stdout", self.output_queue)).start()
            threading.Thread(target=self.read_process_output, args=(self.scons_process.stderr, "stderr", self.output_queue)).start()

            self.check_process()
        except Exception as e:
            messagebox.showerror("错误", f"构建失败:\n{str(e)}")

    def read_process_output(self, stream, stream_type, output_queue):
        for line in iter(stream.readline, ''):
            output_queue.put((stream_type, line))
        stream.close()

    def check_process(self):
        try:
            while not self.output_queue.empty():
                stream_type, line = self.output_queue.get_nowait()
                self.text_box.config(state="normal")
                self.text_box.insert("end", line)
                self.text_box.config(state="disabled")
                self.text_box.yview("end")
        except queue.Empty:
            pass

        return_code = self.scons_process.poll()
        if return_code is None:
            self.master.after(100, self.check_process)
        else:
            if return_code == 0:
                self.run_gui_exe()
            else:
                stderr_output = self.scons_process.stderr.read()
                self.show_error_window(return_code, stderr_output)

    def show_error_window(self, return_code, error_message):
        messagebox.showerror("错误", f"SCons 构建失败，退出代码: {return_code}\n{error_message}")

    def run_gui_exe(self):
        try:
            self.terminate_gui_process()

            gui_exe_path = os.path.join(self.script_dir, "win32_sim", "gui.exe")
            if not os.path.exists(gui_exe_path):
                raise FileNotFoundError(f"gui.exe 未找到在 '{gui_exe_path}'")
            self.gui_process = subprocess.Popen([gui_exe_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            threading.Thread(target=self.read_process_output, args=(self.gui_process.stdout, "stdout", self.output_queue), daemon=True).start()
            threading.Thread(target=self.read_process_output, args=(self.gui_process.stderr, "stderr", self.output_queue), daemon=True).start()

            if self.gui_process.poll() is None:
                print(f"gui.exe started successfully with PID: {self.gui_process.pid}")
            else:
                print("Failed to start gui.exe.")
            # self.check_gui_process()
        except Exception as e:
            messagebox.showerror("错误", f"GUI 启动失败:\n{str(e)}")  

    def check_gui_process(self):
        self.update_text_box()

        return_code = self.gui_process.poll()
        if return_code is None:
            self.master.after(100, self.check_gui_process)
        else:
            if return_code != 0:
                stderr_output = self.gui_process.stderr.read()
                self.show_error_window(return_code, stderr_output)

    def create_widgets(self):
        self.master.grid_rowconfigure(0, weight=0)
        self.master.grid_rowconfigure(1, weight=1)
        self.master.grid_columnconfigure(0, weight=1)
        self.master.grid_columnconfigure(1, weight=0)

        title = tk.Label(self.master, text="GUI Demo", font=('Helvetica', 16))
        title.grid(row=0, column=0, columnspan=2, pady=(20, 10))

        self.canvas = tk.Canvas(self.master)
        scrollbar = tk.Scrollbar(self.master, orient="vertical", command=self.canvas.yview)
        self.scrollable_frame = tk.Frame(self.canvas)

        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            )
        )

        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=scrollbar.set)

        self.canvas.grid(row=1, column=0, sticky='nsew', padx=(50, 0))
        scrollbar.grid(row=1, column=1, sticky='ns')

        self.canvas.bind_all("<MouseWheel>", self.on_mouse_wheel)

        self.buttons = []

        row_index = 0
        for option in self.primary_options:
            button_frame = tk.Frame(self.scrollable_frame, borderwidth=2, relief='flat', padx=10, pady=5, highlightbackground="black", highlightthickness=1)
            button_frame.grid(row=row_index, column=0, padx=10, pady=10, sticky='ew')
            button_frame.grid_columnconfigure(0, weight=1)
            button_frame.option_name = option['name']

            name_label = tk.Label(button_frame, text=option.get('name'), font=('Helvetica', 14, 'bold'), anchor='center')
            name_label.pack(fill='x', pady=(5, 0))

            spacer = tk.Label(button_frame, text=" ", font=('Helvetica', 1))
            spacer.pack(fill='x')

            details_text = f"Resolution: {option.get('resolution')}\nDescription: {option.get('description')}"
            details_label = tk.Label(button_frame, text=details_text, justify="left", wraplength=400, anchor='w')
            details_label.pack(fill='x', padx=5)

            # Ensure all child components propagate the click event to handle_button_click
            for widget in (button_frame, name_label, spacer, details_label):
                widget.bind("<Button-1>", lambda e, opt=option['name']: self.handle_button_click(opt))

            self.buttons.append(button_frame)
            row_index += 1
        self.select_option(self.primary_var.get())

    def select_option(self, option_name):
        self.primary_var.set(option_name)
        for button_frame in self.buttons:
            if button_frame.option_name == option_name:
                button_frame.config(bg="lightblue", relief="solid", bd=2)
                for child in button_frame.winfo_children():
                    if isinstance(child, tk.Label):
                        child.config(bg="lightblue")
            else:
                button_frame.config(bg="SystemButtonFace", relief="flat", bd=2)
                for child in button_frame.winfo_children():
                    if isinstance(child, tk.Label):
                        child.config(bg="SystemButtonFace")

    def handle_button_click(self, option_name):
        self.select_option(option_name)
        self.run_command()

    def on_mouse_wheel(self, event):
        self.canvas.yview_scroll(-1 * (event.delta // 120), "units")

    def cleanup(self):
        if self.scons_process is not None and self.scons_process.poll() is None:
            self.scons_process.terminate()
        if self.gui_process is not None and self.gui_process.poll() is None:
            self.gui_process.terminate()
        if self.info_window is not None and self.safe_winfo_exists(self.info_window):
            self.info_window.destroy()


if __name__ == "__main__":
    
    products_info = {
        "CONFIG_REALTEK_BUILD_GUI_454_454_DEMO": {
            "name": "Watch",
            "resolution": "454x454",
            "description": "Watch application using native C",
            "macro": "CONFIG_REALTEK_BUILD_GUI_454_454_DEMO"
        },
        "CONFIG_REALTEK_BUILD_SCRIPT_AS_A_APP": {
            "name": "86box",
            "resolution": "480x480",
            "description": "86box application using SaaA",
            "macro": "CONFIG_REALTEK_BUILD_SCRIPT_AS_A_APP"
        },
        "CONFIG_REALTEK_BUILD_TEST": {
            "name": "Test",
            "resolution": "480x480",
            "description": "Test application",
            "macro": "CONFIG_REALTEK_BUILD_TEST"
        }
    }

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