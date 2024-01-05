#!/usr/bin/env python

import sys
import git
import re
from jira import JIRA

PROJECT_NAME = ""


def init(repo_addr):
    repo = git.Repo(repo_addr)

    try:
        repo.git.rev_parse('--verify', 'HEAD')
    except (git.exc.GitCommandError, ValueError, IndexError) as e:
        print('check commit message: bypass empty repo checking\n{}'.format(e))
        return None

    return repo


def IsMyProject(title):
    index = title.find(CUSTOMER_NAME)
    if index >= 0:
        return True
    return False


def find_jira(each_line):
    jira_reg = re.compile(r'(\[Jira\])\s*([a-zA-Z_1-9]+-[0-9]+)', re.I)  # (jira:BTSOCTOOL-1578)
    jira_id_list = jira_reg.findall(each_line)
    if jira_id_list:
        for j in jira_id_list:
            return j[1]
    return None


def manager_each_commit(commit_info, jira_list, str_tag_msg):
    lists = commit_info.split('\n')
    if IsMyProject(lists[0]) == False:
        return str_tag_msg

    charStar = '* '
    charDash = '- '
    bodys = lists[1:]
    re_pattern = re.compile('^\d+.')  # from line start, with 2., 3., ....

    for each_line in bodys:
        if len(each_line) == 0:  # skip empty line
            continue
        if each_line.find('Change-Id:') != -1:  # skip change-id line
            continue
        # if each_line.find('\'') != -1 and len(each_line) == 1: # skip
        #    continue

        if each_line.startswith('1.'):  # reshape '1. -> *
            # tag msg
            str_tag_msg += charDash + each_line[2:] + '\n'
            # reshape line
            each_line = charStar + each_line[2:]

        else:
            v = re_pattern.search(each_line)
            if v != None:
                new_start = len(v.group(0))
                # tag msg
                str_tag_msg += charDash + each_line[new_start:] + '\n'
                # reshape line
                each_line = charStar + each_line[new_start:]
            else:
                jira_id = find_jira(each_line)
                if jira_id != None:
                    each_line = r'(https://jira.realtek.com/browse/' + jira_id + r')'
                    jira_list.append(jira_id)

        print(each_line)

    return str_tag_msg


def get_info2(repo, old_commit, new_commit):
    ver_range = old_commit + '~1..' + new_commit
    print('== Version Range:', ver_range)

    commit_list = []
    cur_commit = repo.commit(new_commit)
    print(cur_commit.message)
    commit_list.append(cur_commit)

    while cur_commit != repo.commit(old_commit):
        cur_commit = cur_commit.parents[0]
        print(cur_commit.message)
        commit_list.append(cur_commit)

    jira_list = []
    str_tag_msg = ''
    print('== After Reshape: ')
    for commit in commit_list:
        str_tag_msg = manager_each_commit(commit.message, jira_list, str_tag_msg)

    return (str_tag_msg, jira_list)


def get_info(repo, old_commit, new_commit):
    ver_range = old_commit + '~1..' + new_commit
    print('== Version Range:', ver_range)

    commit_title = repo.git.log(ver_range, "--pretty='%s'")
    print('== Title: ')
    print(commit_title)

    commit_body = repo.git.log(ver_range, "--pretty='%b'")
    print('== Body: ')
    print(commit_body)

    charStar = '* '
    charDash = '- '
    bodys = commit_body.split('\n')
    re_pattern = re.compile('^\d+.')  # from line start, with 2., 3., ....
    jira_reg = re.compile(r'(\[Jira\])\s*([a-zA-Z_1-9]+-[0-9]+)', re.I)  # (jira:BTSOCTOOL-1578)

    jira_list = []
    str_tag_msg = ''
    print('== After Reshape: ')
    for each_line in bodys:
        if len(each_line) == 0:  # skip empty line
            continue
        if each_line.find('Change-Id:') != -1:  # skip change-id line
            continue
        if each_line.find('\'') != -1 and len(each_line) == 1:  # skip
            continue

        if each_line.startswith('\'1.'):  # reshape '1. -> *
            # tag msg
            str_tag_msg += charDash + each_line[3:] + '\n'
            # reshape line
            each_line = charStar + each_line[3:]

        else:
            v = re_pattern.search(each_line)
            if v != None:
                new_start = len(v.group(0))
                # tag msg
                str_tag_msg += charDash + each_line[new_start:] + '\n'
                # reshape line
                each_line = charStar + each_line[new_start:]
            else:
                jira_id_list = jira_reg.findall(each_line)
                if jira_id_list:
                    for j in jira_id_list:
                        each_line = r'(https://jira.realtek.com/browse/' + j[1] + r')'
                        jira_list.append(j[1])

        print(each_line)

    return (str_tag_msg, jira_list)


def add_tag(tag_name, more_info):
    str_sample = 'Tag for soc tool release' + '\n\nRelease Notes:\n' + more_info
    print('\n== Check Tag Message: ')
    print(str_sample)

    y_or_n = input('Key in \'yes\' to actually add tag....')
    if y_or_n == 'yes':
        repo.git().tag('-a', tag_name, '-m', str_sample)
        print('Has add a tag')

    return


def GetJiraReporterAndLabels(user, password, list_jira_id):
    print('Start Login Jira')
    jira = JIRA(server='https://jira.realtek.com', basic_auth=(user, password))
    for jira_id in list_jira_id:
        issue = jira.issue(jira_id)

        print('ID is: ', jira_id, 'Label is: ', issue.fields.labels, 'Reporter: ', issue.fields.reporter)
        print('Summary is: ', issue.fields.summary)
        print('Key is: ', issue.key)

        JiraDetailDic = {
            "Reporter": issue.fields.reporter,
            "Labels": issue.fields.labels
        }


def GetJiraIssueInfo(jira, jira_id):
    issue = jira.issue(jira_id)
    # print('ID is: ', jira_id, 'Label is: ', issue.fields.labels, 'Reporter: ', issue.fields.reporter)
    # print('Summary is: ', issue.fields.summary)
    dictInfo = {
        # "ID": jira_id,
        "Chip": [],
        "Reporter": str(issue.fields.reporter),
        "Label": '',
        "Summary": issue.fields.summary
    }

    summary = issue.fields.summary
    if 'BBPro2' in summary:
        dictInfo["Chip"].append('BBPro2')

    if 'BBLite D Cut' in summary:
        dictInfo["Chip"].append('BBLite D Cut')

    if 'BBLite C Cut' in summary:
        dictInfo["Chip"].append('BBLite C Cut')
    if 'BB2' in summary:
        dictInfo["Chip"].append('BB2')

    if 'NormalRelease' in issue.fields.labels:
        dictInfo["Label"] = 'NormalRelease'  # .append('NormalRelease')

    dictInfo['Link'] = r'https://jira.realtek.com/browse/' + jira_id
    return dictInfo


'''
| Feature Log  | Chip of feature  |  JIRA  |  JIRA Label | JIRA Reporter | Test Report |
| Modify AVCC DRV related name. | BBLite D Cut/BBPro2 | https://jira.realtek.com/browse/BTSOCTOOL-65 | Normal Release | kk | |
| Modify AVCC DRV related name2. | BBLite D Cut/BBPro2 | https://jira.realtek.com/browse/BTSOCTOOL-64 | Normal Release | kk2 | |
'''


def Reshape2(dictInfo):
    l = ['', '*', ",".join(dictInfo['Chip']), dictInfo['Link'], dictInfo['Label'], dictInfo['Reporter'], '-', '']
    s = " | ".join(
        l)  # "|" + dictInfo['Summary'] + "|" + ",".join(dictInfo['Chip']) + "|" + dictInfo['Link'] + "|" + dictInfo['Label'] + "|" + dictInfo['Reporter'] + "|"
    print(s)


def GetJiraAllIssueInfo(user, password, list_jira_id):
    print('== Start Login Jira')
    jira = JIRA(server='https://jira.realtek.com', basic_auth=(user, password))

    md = r' | Release Note  | Support Chip  |  JIRA  |  JIRA Label | JIRA Reporter | Test Report |'  # mark down string
    print(md)

    for jira_id in list_jira_id:
        info = GetJiraIssueInfo(jira, jira_id)
        Reshape2(info)


def NotifyJiraAllIssue(user, password, list_jira_id, tag):
    print('== Start Login Jira 2')
    jira = JIRA(server='https://jira.realtek.com', basic_auth=(user, password))

    for jira_id in list_jira_id:
        issue = jira.issue(jira_id)

        mailAddr = issue.fields.creator.emailAddress

        comment1 = "Dear [~{}]\n".format(mailAddr[0:mailAddr.find(r'@')])
        comment2 = "Release version: {}\n".format(tag)
        comment_detail = comment1 + comment2 + r"请帮忙确认功能是否如预期，谢谢"
        print(comment_detail)

        jira.add_comment(issue, comment_detail)
        jira.assign_issue(issue, mailAddr[0:mailAddr.find(r'@')])


def GetLastReleaseInfo(user, password, release_jira_id):
    print('== Start Login Jira 3')
    jira = JIRA(server='https://jira.realtek.com', basic_auth=(user, password))
    issue = jira.issue(release_jira_id)

    sort_comment = sorted(issue.fields.comment.comments, key=lambda comment: comment.created, reverse=True)
    print(sort_comment[0].body)


'''  ANC tag and project
map_relation = {
        "RAMP1": "AncAutoMP",
        "RAMP2": "AncAutoMPBBpro2",
        "bbpro2": "AncDesignTool",
        "BBLite": "AncConfigTool",
    }
'''

if __name__ == '__main__':

    isBranchMaster = True  # To Modify...
    if isBranchMaster == True:
        repo_addr = "D:\\MyProjects\\realgui\\HoneyGUI\\.git"  # To Modify...
        tag_name = 'v1.0.12.0'  # To Modify...
    else:
        repo_addr = "D:\Git\AssemblyBB2\AssemblyTest\.git"  # To Modify...
        tag_name = 'annie_test'  # To Modify...

    new_commit = '97aff966cfa0821cfe27137228a3992dbfcaebe2'  # To Modify...
    old_commit = '97aff966cfa0821cfe27137228a3992dbfcaebe2'  # To Modify...

    CUSTOMER_NAME = ""  # To Modify...

    repo = init(repo_addr)
    if repo == None:
        exit()

    # Step 1
    tag_msg, jira_list = get_info2(repo, old_commit, new_commit)  # For Anc related, AssemblyTest repo

    # Step 2
    add_tag(tag_name, tag_msg)

    # Step 3
    # file your own name and password
    user = r'triton_yu'  # Modify
    password = r'Atsea666666666666'  # Modify
    if True:  # False:
        GetJiraAllIssueInfo(user, password, jira_list)

    # Step 4
    # Get release package info
    y_or_n = input('Key in \'y\' to get JENKINS release info....')
    if y_or_n == 'y':
        release_jira_id = r'BTTOOLSDK-81'  # To Modify
        GetLastReleaseInfo(user, password, release_jira_id)

    # Step 5
    # Add jira comment and assign to reporter
    if False:
        y_or_n = input('Key in \'y\' to actually add jira comment....')
        if y_or_n == 'y':
            NotifyJiraAllIssue(user, password, jira_list, tag_name)


