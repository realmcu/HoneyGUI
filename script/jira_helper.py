#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import json
import os
import sys
import time
import re
try:
    from jira import JIRA
except:
    pass


class Jira():

    def __init__(self):
        self.connect()
    
    def connect(self):
        username = os.getenv("JIRA_USER")
        password = os.getenv("JIRA_PWD")
        if not username or not password:
            print("get jira user info error, please configure jira user credential")
            return
        try:
            self.jira = JIRA(server='https://jira.realtek.com', basic_auth=(username, password))
            print("Connect jira successed")
        except Exception as e:
            print('ERROR: connect jira error {}'.format(e))

    def disconnect(self):
        try:
            self.jira.kill_session()
        except Exception as e:
            print('ERROR: disconnect jira error {}'.format(e))

    def download_attachements_by_jira_id(self, jira_id, file_name, download_path, check_status=True):
        task_issue = self.jira.issue(jira_id)
        attachments = sorted(task_issue.fields.attachment, key=lambda attachment: attachment.created, reverse=True)

        for attachment in attachments:
            if attachment.filename == file_name:
                break
        else:
            if check_status:
                sys.exit('cannot found {} in {}'.format(file_name, task_issue.key))
            else:
                print('cannot found {} in {}'.format(file_name, task_issue.key))

        dest_file_full_path = os.path.join(download_path, attachment.filename)
        with open(dest_file_full_path, 'wb') as f:
            print('download attachment {}, created in {}.'.format(attachment.filename, attachment.created))
            f.write(attachment.get())

        return dest_file_full_path

    def find_packages_from_jira(self, jira_id, pattern):
        package_list = list()
        task_issue = self.jira.issue(jira_id)
        attachments = sorted(task_issue.fields.attachment, key=lambda attachment: attachment.created, reverse=True)

        for attachment in attachments:
            if re.search(pattern, attachment.filename):
                package_list.append(attachment.filename)
        return package_list


    def get_attachment_link(self, jira_id, file_name):
        task_issue = self.jira.issue(jira_id)
        attachments = sorted(task_issue.fields.attachment, key=lambda attachment: attachment.created, reverse=True)
        for attachment in attachments:
            if file_name == attachment.filename:
                return attachment.content
        return ""