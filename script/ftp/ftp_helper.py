import os
import sys
import time
import json
import paramiko
from stat import S_ISDIR as isdir

class SSH():
    def __init__(self, host, port, key):
        self.host = host
        self.port = port
        self.username, self.password = self.get_ftp_credentials(key)
        self.ssh_client = None
        self.trans_client = None
        self.sftp = None

    def get_ftp_credentials(self, key):
        if key == "RTFTP":
            return os.getenv("RT_USER"), os.getenv("RT_PWD")
        else:
            return os.getenv("RS_USER"), os.getenv("RS_PWD")

    def exec_command(self, command, timeout=6000):
        print(command, flush=True)
        self.ssh_client = paramiko.SSHClient()
        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        if not self.username or not self.password:
            print("get ftp user info error, please configure ftp user credential")
            return
        self.ssh_client.connect(self.host, self.port, self.username, self.password)
        stdin, stdout, stderr = self.ssh_client.exec_command(command, timeout=timeout)
        out_list = stdout.readlines()
        self.ssh_client.close()

        return out_list

class SFTP(SSH):
    def connect(self):
        retry_count = 3
        if not self.username or not self.password:
            print("get ftp user info error, please configure ftp user credential")
            return
        for i in range(retry_count):
            try:
                print("Connect ftp...", flush=True)
                self.trans_client = paramiko.Transport((self.host, self.port))
                self.trans_client.banner_timeout = 60
                self.trans_client.connect(username=self.username, password=self.password)
                self.sftp = paramiko.SFTPClient.from_transport(self.trans_client)
                print("Connect ftp successed", flush=True)
                break
            except Exception as e:
                print("Connect ftp server exception: {}".format(e))
                time.sleep(10)

    def disconnect(self):
        if self.trans_client:
            try:
                self.trans_client.close()
            except Exception as e:
                print(e)
        if self.ssh_client:
            try:
                self.ssh_client.close()
            except Exception as e:
                print(e)

    def upload(self, local, remote, cwd=None):
        remote = remote.replace('\\','/').strip("\n")
        cwd = cwd or get_ftp_folder()[0]
        try:
            self.sftp.chdir(cwd)
        except:
            self.make_dir(cwd)
            self.sftp.chdir(cwd)
        if remote != cwd: 
            self.make_dir(remote)
            self.sftp.chdir(cwd)
        remote = os.path.join(cwd, remote).replace('\\','/')
        def _is_exists(path, function):
            path = path.replace('\\','/')
            try:
                function(path)
            except:
                return False
            else:
                return True
        def _copy(local, remote):
            _is_exists(remote, function=self.sftp.mkdir)
            if _is_exists(remote, function=self.sftp.chdir):
                filename = os.path.basename(os.path.normpath(local))
                remote = os.path.join(remote, filename).replace('\\','/')
            if os.path.isdir(local):
                for file in os.listdir(local):
                    localfile = os.path.join(local, file).replace('\\','/')
                    _copy(local=localfile, remote=remote)
            if os.path.isfile(local):
                try:
                    self.sftp.put(local, remote)
                except Exception as error:
                    print("upload {} ==> {} FAILED: {}".format(local, remote, error), flush=True)
                else:
                    print("upload {} ==> {} SUCCESSED".format(local, remote), flush=True)
        if not _is_exists(local, function=os.stat):
            print("No such file or directory {} in local".format(local), flush=True)
            return False
        _copy(local=local, remote=remote)
        try:
            self.sftp.chdir(cwd)
        except:
            pass

    def _check_local(self, local):
        if not os.path.exists(local):
            try:
                os.mkdir(local)
            except IOError as err:
                print(err)
    
    def download(self, remote, local):
        remote = remote.replace('\\','/').strip("\n")
        try:
            result = self.sftp.stat(remote)
        except IOError as err:
            print(err, flush=True)
        else:
            self._check_local(local)
            if isdir(result.st_mode):
                dirname = os.path.basename(os.path.normpath(remote))
                local = os.path.join(local, dirname)
                for file in self.sftp.listdir(remote):
                    sub_remote = os.path.join(remote, file)
                    sub_remote = sub_remote.replace('\\','/')
                    print("download {} start".format(sub_remote), flush=True)
                    self.download(sub_remote, local)
            else:
                if os.path.isdir(local):
                    local = os.path.join(local,os.path.basename(remote))
                try:
                    print("download {} start".format(remote), flush=True)
                    self.sftp.get(remote, local)
                except IOError as err:
                    print(err, flush=True)
                else:
                    print("download {} ==> {} SUCCESSED".format(remote, local), flush=True)

    def make_dir(self, remote):
        remote = remote.replace('\\','/').strip("\n")
        remote_list = remote.split("/")
        cwd = get_ftp_folder()[0]
        try:
            self.sftp.chdir(cwd)
        except:
            pass
        for p in remote_list:
            if not p:
                continue
            try:
                self.sftp.chdir(p)
            except:
                self.sftp.mkdir(p)
                self.sftp.chdir(p)
        try:
            self.sftp.chdir(cwd)
        except:
            pass
        print("Make directory: {} success".format(remote), flush=True)

    def unzip(self, zip_file, unzip_path):
        try:
            self.make_dir(unzip_path)
            self.exec_command('tar -zxvf {} -C {}'.format(zip_file.replace('\\','/'), os.path.join(get_ftp_folder(local=True)[0], unzip_path).replace('\\','/')))
            print("Unzip {} success".format(zip_file), flush=True)
        except Exception as e:
            print("Unzip {} failed: {}".format(zip_file, e), flush=True)

    def zip(self, zip_file, zip_path):
        try:
            self.exec_command('tar -czf {} {}'.format(zip_file.replace('\\','/'), os.path.join(get_ftp_folder(local=True)[0], zip_path).replace('\\','/')))
            print("zip {} success".format(zip_file), flush=True)
        except Exception as e:
            print("zip {} failed: {}".format(zip_file, e), flush=True)
    
    def remove(self, dir):
        try:
            self.exec_command('rm -rf {}'.format(dir.replace('\\','/')))
            print("remove {} success".format(dir), flush=True)
        except Exception as e:
            print("remove {} failed: {}".format(dir, e), flush=True)

    def mv(self, src, dest):
        local_ftp_folder = get_ftp_folder(local=True)[0]
        try:
            self.exec_command('mv {}/* {}'.format(os.path.join(local_ftp_folder, src).replace('\\','/'), os.path.join(local_ftp_folder, dest).replace('\\','/')))
            print("move {} to {} success".format(src, dest), flush=True)
        except Exception as e:
            print("move {} to {} failed: {}".format(src, dest, e), flush=True)

    def rm(self, dir):
        local_ftp_folder = get_ftp_folder(local=True)[0]
        try:
            self.exec_command('rm -rf {}'.format(os.path.join(local_ftp_folder, dir).replace('\\','/')))
            print("remove {} success".format(dir), flush=True)
        except Exception as e:
            print("remove {} failed: {}".format(dir, e), flush=True)

    def find(self, file_name):
        find_list = list()
        result_list = self.exec_command('find {} -name {}'.format(os.path.dirname(file_name).replace('\\','/'), os.path.basename(file_name)))
        
        for f in result_list:
            if os.path.splitext(f)[1] not in file_name:
                f = "/".join(f.strip("\n").split("/")[2:])
                find_list.append(f)
        return find_list

    def is_exist(self, remote):
        remote = remote.replace('\\','/')
        local = os.path.join(os.path.dirname(__file__), os.path.basename(remote))
        try:
            self.sftp.get(remote, local)
            return True
        except IOError as err:
            print(err, flush=True)
            return False

    def exists(self, path):
        cur = self.sftp.getcwd()
        print("cur: {}".format(cur))
        path = path.replace('\\','/')
        try:
            self.sftp.chdir(path)
            self.sftp.chdir(cur)
            return True
        except:
            return False

def rs_ftp_host():
    return "wn-bt-nas.realsil.com.cn", 22, "RSFTP"

def rt_ftp_host():
    return "wn-bt-nas.realtek.com", 22, "RTFTP"

def get_ftp_host():
    if "tw-" in os.getenv("NODE_NAME"):
        return rt_ftp_host(), rs_ftp_host()
    else:
        return rs_ftp_host(), rt_ftp_host()

def rt_ftp_server(local=False):
    relative_path = r'/RT_LOCAL/JENKINS/{}/{}'.format(os.getenv("JOB_BASE_NAME"), os.getenv('BUILD_NUMBER')).replace('\\','/')
    return r"/volume1{}".format(relative_path) if local else relative_path

def rs_ftp_server(local=False):
    relative_path = r'/RS_LOCAL/JENKINS/{}/{}'.format(os.getenv("JOB_BASE_NAME"), os.getenv('BUILD_NUMBER')).replace('\\','/')
    return r"/volume1{}".format(relative_path) if local else relative_path

def get_ftp_folder(local=False):
    if "tw-" in os.getenv("NODE_NAME"):
        return rt_ftp_server(local=local), rs_ftp_server(local=local)
    else:
        return rs_ftp_server(local=local), rt_ftp_server(local=local)

def get_ftp_base():
    if "tw-" in os.getenv("NODE_NAME"):
        return r"/RT_LOCAL", r"/RS_LOCAL"
    else:
        return r"/RS_LOCAL", r"/RT_LOCAL"

def get_sync_ftp_base():
    if "tw-" in os.getenv("NODE_NAME"):
        return r"/RT", r"/RS"
    else:
        return r"/RS", r"/RT"

def get_task_proportion_relative_path():
    return "task_proportion", "task_proportion"

def get_heap_analyzer_relative_path():
    return "heap_analyse", "heap_analyse"

def get_minimun_heap_relative_path():
    return "minimum_heap"
