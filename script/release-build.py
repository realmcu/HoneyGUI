import argparse
import os
import git
import sys
import re
import json
import subprocess
import os
import shutil
from jira import JIRA

class WindowsToolRelease():
    def __init__(self):
        self.tool_name = os.environ['TOOLRELEASENAME']
        self.tag_name = os.environ['TOOLRELEASEVERSION']
        self.distribution_directory = r'Dist'
        self.cwd = os.getcwd()
        self.platform = os.environ['PLATFORMRELEASED']
        self.release_dashboard = r'BTTOOLSDK-1'
        self.jira_project_key = r'BTTOOLSDK'
        self.jira_issue_reporter = r'bluetooth_jenkins'
        self.labels = [r'SOCTOOL']
        self.owner = "catherine_guo"
        self.platform_separate = False
        self.jira_issue_group_watcher = [{'name': 'JR_BT_Windows'}, {'name': 'JR_BT_ANDROID_LINUX'}]
        self.release_notes = ""
        self.jira_issue_description_template  = r'''Here is the {} official release site.

You shall NEVER produce any personal or private tools and release them to the public.

Each platform create a subtask automatically and push the tagged tool as the subtask's attachment.

You are encouraged to develop and perform more tests on these tools.
'''
        self.jira_subissue_comments_version = '''Hi ALL,

*{1} {2}* {0} is released.
Public version: {3}
Internal version: {4}
{5}

The key highlights of this release include:
''' # {0} platform {1} TOOL_NAME, {2} TAG_NAME, {3} attachment content {4} internal attachment 

    def empty_folder(self, dir_path):
        if os.path.isdir(dir_path):
            shutil.rmtree(dir_path)
        os.makedirs(dir_path)

    def get_tag_item(self):
        check_version = re.search(r'v(\d+)\.(\d+)\.(\d+)\.(\d+)(.*)', self.tag_name, flags=re.I)
        if check_version:
            return (check_version.group(1), check_version.group(2), check_version.group(3), check_version.group(4), check_version.group(5))
        else:
            sys.exit("Script error, {} can not find first segment".format(self.tag_name))

    def jenkins_check_env(self):
        jira_subissue_comments_release_notes = ""
        repo = git.Repo(search_parent_directories=True)
        try:
            repo.git.rev_parse('--verify', 'HEAD')
        except (git.exc.GitCommandError, ValueError, IndexError) as e:
            sys.exit('jenkins_check_env: error empty repo {}'.format(e))

        kw_releasenotes = r'Release Notes:$'
        markdown_list = r'^\s{4}-\s'
        tag_message = repo.git.tag('--list', self.tag_name, '-n10000').strip('\'')

        try:
            start = re.search(kw_releasenotes, tag_message, re.MULTILINE).span()[1]
            jira_subissue_comments_release_notes = re.sub(markdown_list, r'- ', tag_message[start:], flags=re.MULTILINE)
            self.release_notes = jira_subissue_comments_release_notes
        except AttributeError:
            sys.exit('jenkins_check_env: error no \'Release Notes:\' line in tag commit \'{}\''.format(tag_message))

    def ms_build_project(self, project_path, configuration):
        '''Build visual studio project'''
        msbuild_path = os.environ.get("MS_BUILD")
        if not msbuild_path:
            sys.exit("Can not find ms build path")
        cmd_build = [msbuild_path, r'/t:Rebuild', configuration, project_path]
        print('ms_build_project: command {}'.format(cmd_build))
        try:
            output = subprocess.check_output(cmd_build, universal_newlines=True, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            sys.exit('ms_build_project: compiling \'{}\' output error ->\n{}'.format(project_path, e.output))

        print('ms_build_project: compiling \'{}\' output pass ->\n{}'.format(project_path, output))


    def uncomment_code_line(self, line_content, file_path):
        '''
        remove // 
        '''
        with open(file_path, encoding='utf-8',newline='', errors='surrogateescape', mode='r') as fd:
            all_lines = fd.readlines()
            for i in range(len(all_lines)):
                if line_content in all_lines[i]:
                    print(f"find          -> {all_lines[i]}")
                    all_lines[i] = re.sub(r'^(\s*)//', lambda obj:obj.group(1), all_lines[i])
                    print(f"after replace -> {all_lines[i]}")

        with open(file_path, encoding='utf-8',newline='', errors='surrogateescape', mode='w') as fd:
            fd.writelines(all_lines)


    def do_copy(self, src_list, dest_dir):
        """
        copy all file/dir in src_list inside the dest_dir
        """
        if not os.path.exists(dest_dir):
            os.mkdir(dest_dir)
        for t_s in src_list:
            t_dest = os.path.join(dest_dir, os.path.basename(t_s))
            if os.path.exists(t_dest):
                sys.exit(f"do_copy fail -> {t_dest} exists")
            if os.path.isdir(t_s):
                print("copy foler", t_s, "->", t_dest)
                shutil.copytree(t_s, t_dest)
            else:
                print("copy file", t_s, "->", dest_dir)
                shutil.copy2(t_s, dest_dir)

    def before_build(self):
        pass

    def jenkins_build_tool(self):
        pass

    def jenkins_test_tool(self):
        pass

    def jenkins_archive_tool(self):
        pass

    def jenkins_push_tool(self):
        pass

    def get_tool_release_summary(self, platform=""):
        if platform:
            return self.tool_name + f" {platform} Release Task"
        return self.tool_name + " Release Task"

    def get_jira_ins(self):
        user, password = os.environ.get("JIRA_USR_NAME"), os.environ.get("JIRA_PWD")
        jira = JIRA(server='https://jira.realtek.com', basic_auth=(user, password))
        return jira
    
    def print_git_diff(self):
        repo = git.Repo(search_parent_directories=True)
        print('diff ->\n{}'.format(repo.git.diff()), flush=True)

    def get_tool_release_task_jira(self):
        """
        Tool dashboard
        tool task
        tool platfrom subtask
        """
        jira = self.get_jira_ins()
        dashboard_issue = jira.issue(self.release_dashboard)

        tool_summary = self.get_tool_release_summary()
        create_task = True

        for issuelink in dashboard_issue.fields.issuelinks:
            if issuelink.type.outward == 'contains':
                if (issuelink.outwardIssue.key.split('-')[0] == self.jira_project_key and
                    issuelink.outwardIssue.fields.summary == tool_summary and
                    issuelink.outwardIssue.fields.issuetype.name == 'Task'):
                    try:
                        task_issue = jira.issue(issuelink.outwardIssue.key)
                        if (task_issue.fields.reporter.name == self.jira_issue_reporter):
                            create_task = False
                            break
                    except Exception as e:
                        jira.kill_session()
                        sys.exit('jira_push_tool: error {}'.format(e))

        # Create tool task issue if not found
        print("create_task:",create_task)

        if create_task == True:
            issue_dict = {
                'project': {'key': self.jira_project_key},
                'summary': tool_summary,
                'description': self.jira_issue_description_template.format(self.tool_name),
                'labels': self.labels,
                'issuetype': {'name': 'Task'},
                'assignee': {'name': self.owner},
                'customfield_10601': [{'value': 'General'}], # RTL NO
                'customfield_10602': [{'value': 'SoC'}], # Chip Type
                'customfield_10603': {'value': 'SW'},  # Function
                'customfield_10604': {'value': 'General'}, # OS
                'customfield_10605': {'value': 'BT-Tool'}, # Application
                'customfield_10606': [{'value': 'Internal'}], # Customer
                'customfield_11801': self.jira_issue_group_watcher, # Group Watcher
            }
            try:
                task_issue = jira.create_issue(fields=issue_dict)
                jira.create_issue_link('contains', dashboard_issue, task_issue)
                print(f'create task {task_issue.key} by jenkins')
            except Exception as e:
                jira.kill_session()
                sys.exit('jira_push_tool: error {}'.format(e))
        if not self.platform_separate:
            print("different platform, same tool")
            return jira, task_issue, ""

        print("different platform, different tool")
        subtask_summary = self.get_tool_release_summary(platform=self.platform)
        for subtask in task_issue.fields.subtasks:
            if (subtask.key.split('-')[0] == self.jira_project_key and
                subtask.fields.summary == subtask_summary and
                subtask.fields.issuetype.name == 'Sub-task'):
                print(f"find {self.tool_name} {self.platform} release jira {subtask.key}, return")
            return jira, subtask, self.platform

        subissue_dict = {
            'project': {'key': self.jira_project_key},
            'parent': {'key': task_issue.key},
            'summary': subtask_summary,
            'description': self.jira_issue_description_template.format(self.tool_name + " " + self.platform),
            'labels':self.labels,
            'issuetype': {'name': 'Sub-task'},
            'assignee': {'name': self.owner},
            'customfield_10601': [{'value': 'General'}], # RTL NO
            'customfield_10602': [{'value': 'SoC'}], # Chip Type
            'customfield_10603': {'value': 'SW'},  # Function
            'customfield_10604': {'value': 'General'}, # OS
            'customfield_10605': {'value': 'BT-Tool'}, # Application
            'customfield_10606': [{'value': 'Internal'}], # Customer
            'customfield_11801': self.jira_issue_group_watcher # Group Watcher
        }
        try:
            subtask_issue = jira.create_issue(fields=subissue_dict)
            print(f'create subtask by jenkins {subtask_issue.key}')
        except Exception as e:
            jira.kill_session()
            sys.exit('jira_push_tool: error {}'.format(e))

        return jira, subtask_issue, self.platform
        

class HoneyGUIRelease(WindowsToolRelease):
    def __init__(self):
        WindowsToolRelease.__init__(self)
        self.owner = "mengfei_gao"
        self.platform_separate = False
        self.bin_dir = os.path.join(self.cwd, r'gui_engine\rtk_gui_sim')
        self.mingw64_path = os.environ.get("MINGW64")
        if not self.mingw64_path:
            sys.exit(f"Can not find env MINGW64")
        os.environ["PATH"] += os.pathsep + os.path.join(self.mingw64_path,"bin")

    def before_build(self):

        self.print_git_diff()

    def jenkins_build_tool(self):
        self.before_build()
        try:
            subprocess.check_call(["scons"], universal_newlines=True, stderr=subprocess.STDOUT, cwd=os.path.join(self.cwd, r'gui_engine\rtk_gui_sim'))
        except subprocess.CalledProcessError as e:
            sys.exit('scons: compiling error code ->\n{}'.format(e.returncode))

    def get_pack_name(self):
        #  v1.0.x.0
        #  GUI simulator-vx.zip
        tag_item = self.get_tag_item()
        pack_name = f'{self.tool_name}-{self.tag_name}'
        return pack_name, pack_name + '.zip'

    def jenkins_archive_tool(self):
        self.empty_folder(os.path.join(self.cwd, self.distribution_directory))
        src_list = [
            os.path.join(self.bin_dir, "SDL2.dll"),
            os.path.join(self.bin_dir, "gui.exe"),
            os.path.join(self.mingw64_path, r"x86_64-w64-mingw32\lib\libstdc++-6.dll"),
            os.path.join(self.mingw64_path, r"x86_64-w64-mingw32\lib\libgcc_s_sjlj-1.dll"),
            os.path.join(self.mingw64_path, r"x86_64-w64-mingw32\lib\libwinpthread-1.dll")
        ]
        self.do_copy(src_list, self.distribution_directory)
        pack_name , _ = self.get_pack_name() 
        shutil.make_archive(base_name=pack_name, format='zip', root_dir=self.distribution_directory)

    def jenkins_push_tool(self):
        jira, subtask, t_platform = self.get_tool_release_task_jira()
        _ , pack_name = self.get_pack_name() 
        attachment = jira.add_attachment(issue=subtask, attachment=os.path.join(self.cwd, pack_name))
        print(f"upload {os.path.join(self.cwd, pack_name)} to {subtask.key}")
        comment = self.jira_subissue_comments_version.format(t_platform, self.tool_name, self.tag_name, attachment.content, "NA", "")
        comment = comment + self.release_notes
        jira.add_comment(subtask, comment)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='GUI Design Tool release build script')
    parser.add_argument('-s', '--step', default='build', choices=['build', 'test', 'archive', 'push'], help='Select release build step')
    step = parser.parse_args().step

    honey_gui_release = HoneyGUIRelease()
    honey_gui_release.jenkins_check_env()

    if step == 'build':
        honey_gui_release.jenkins_build_tool()
    elif step == 'test':
        honey_gui_release.jenkins_test_tool()
    if step == 'archive':
        honey_gui_release.jenkins_archive_tool()
    elif step == 'push':
        honey_gui_release.jenkins_push_tool()
