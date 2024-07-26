import sys
import time
import signal
import subprocess
import re
import os
from threading import Timer

class ToolBase():
    def __init__(self, cmd_line, **kwargs):
        self.cmd_line = cmd_line
        self.delay = kwargs.get("delay", 0)
        self.wait = kwargs.get("wait", True)
        self.exit_once_failed = kwargs.get("exit_once_failed", True)
        self.timeout = kwargs.get("timeout", None)
        self.wait_event = kwargs.get("wait_event", None)
        self.process = kwargs.get("process", None)
        self.temp_pip_name = kwargs.get("temp_pip_name", None)
        self.msg = kwargs.get("msg", "exit")

    def tool_run(self):
        '''Run cmd line tool'''
        '''
        wait_event:{
            message: "abc",
            callback: callbackFunc
        }

        '''
        if not self.cmd_line:
            sys.exit("No command line to run")

        print('tool_run cmd \n{}'.format(' '.join(self.cmd_line)), flush=True)

        if self.wait:
            try:
                process = subprocess.Popen(self.cmd_line, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                        universal_newlines=True, creationflags=subprocess.CREATE_NEW_PROCESS_GROUP) 
                time.sleep(self.delay)
                watchdog = Watchdog(7200) #timeout is 2h
                while True:
                    sys.stdout.flush()
                    line = process.stdout.readline()
                    if line == '' and process.poll() != None:
                        break
                    if line != '':
                        print(line.replace('\n',''), flush=True)
                        if self.wait_event and self.wait_event.get("message") and self.wait_event.get("message") in line and callable(self.wait_event.get("callback")):
                            self.wait_event["callback"](line)
                        watchdog.reset()
                    if re.search(r'Error: ', line) or re.search(r'Download : Failed', line) or re.search(r'reboot fail', line) or re.search(r'Fail at', line):
                        try:
                            with open(os.path.join(os.path.join(os.path.dirname(__file__), r'../test/error.log')), 'a') as f:
                                f.write(line)
                        except Exception as e:
                            print("Write file exception:{}".format(e))
                    if re.search("java.lang.InterruptedException|java.nio.channels.ClosedChannelException|hudson.remoting.ChannelClosedException", line):
                        raise subprocess.CalledProcessError(-100, process.args, output=process.stdout, stderr=process.stderr)
                returncode = process.wait(timeout = self.timeout)
                if returncode:
                    raise subprocess.CalledProcessError(returncode, process.args, output=process.stdout, stderr=process.stderr)
            except subprocess.TimeoutExpired as e:
                watchdog.stop()
                sys.exit('timeout for cmd {}'.format(' '.join(self.cmd_line)))
            except subprocess.CalledProcessError as e:
                if self.exit_once_failed:
                    watchdog.stop()
                    sys.exit('cmd {} failed, return code {}'.format(' '.join(self.cmd_line), e.returncode))
                else:
                    print('cmd {} failed, return code {}'.format(' '.join(self.cmd_line), e.returncode))
                    watchdog.stop()
                    return False
            except Watchdog:
                watchdog.stop()
                sys.exit('There is no log output more than 2h, stop {}'.format(' '.join(self.cmd_line)))
            else:
                print('wait cmd {}, delay {}, output success'.format(' '.join(self.cmd_line), self.delay), flush=True)
                watchdog.stop()
                return True
        else:
            process = subprocess.Popen(self.cmd_line, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                        universal_newlines=True, creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)

            time.sleep(self.delay)
            retcode = process.poll()

            if retcode:
                raise subprocess.CalledProcessError(retcode, process.args, output=process.stdout, stderr=process.stderr)
            else:
                print('nowait cmd, delay {}'.format(self.delay), flush=True)
                return process

    @staticmethod
    def tool_stop(process):
        try:
            process.send_signal(signal.CTRL_BREAK_EVENT)
            time.sleep(2)
            process.kill()
            process.wait()
        except Exception as e:
            print('{} stop tool exception: {}'.format(time.strftime("%y_%m_%d_%H_%M_%S", time.localtime()), e))

    @staticmethod
    def send_exit_to_tool_as_pipclient(temp_pip_name, msg="exit"):
        import win32file

        if not temp_pip_name:
            return
        pipe_name = r"\\.\pipe\{}".format(temp_pip_name)
        #PIPE_NAME = r'\\.\pipe\test_pipe'

        file_handle = win32file.CreateFile(pipe_name,
                                        win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                                        win32file.FILE_SHARE_WRITE, None,
                                        win32file.OPEN_EXISTING, 0, None)
        try:
            win32file.WriteFile(file_handle, msg.encode('utf-8'))
            print("send exit to tool", flush = True)
            time.sleep(0.1)
        finally:
            try:
                win32file.CloseHandle(file_handle)
            except:
                print('client close Hanle fail')


class Watchdog(Exception):
    def __init__(self, timeout, userHandler=None):  # timeout in seconds
        self.timeout = timeout
        self.handler = userHandler if userHandler is not None else self.defaultHandler
        self.timer = Timer(self.timeout, self.handler)
        self.timer.start()

    def reset(self):
        self.timer.cancel()
        self.timer = Timer(self.timeout, self.handler)
        self.timer.start()

    def stop(self):
        self.timer.cancel()

    def defaultHandler(self):
        sys.exit('There is no log output more than 2h, stop test tool')
