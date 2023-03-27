import java.text.SimpleDateFormat

// branches defined in BRANCH_CUT_MAP will be tested
jenkins_test_build_name = ""
jenkins_test_script_name = ""


LOWERSTACK_PATCH = "lowerstack-patch"
LOWERSTACK_PATCH_PATH_RELATIVE_TO_REPO_HOME = ''
LOWERSTACK_PATCH_PATH_RELATIVE_TO_WORKSPACE = ""
LOWERSTACK_PATCH_BRANCH = ''

LOWERSTACK_ROM = "lowerstack-rom"
LOWERSTACK_ROM_PATH_RELATIVE_TO_REPO_HOME = ''
LOWERSTACK_ROM_PATH_RELATIVE_TO_WORKSPACE = ""
LOWERSTACK_ROM_BRANCH = ''

SDK = 'sdk'
SDK_PATH_RELATIVE_TO_REPO_HOME = ''
SDK_PATH_RELATIVE_TO_WORKSPACE = ''
SDK_BRANCH = ''

ROM = 'rom'
ROM_PATH_RELATIVE_TO_REPO_HOME = ''
ROM_PATH_RELATIVE_TO_WORKSPACE = ''
ROM_BRANCH = ''



ROM_BRANCH_CUT_MAP = [
    "_bblite2-dev":[
        "cutThrottle" : "BBLite2FPGATest",
        "slave_label_prefix": "bb2-fpga",
        "branchMapping": ""
    ]
]// branches here will be tested

Need_Test = true
temp_label_prefix = ""
test_module_map = []
retryCount = 3

def set_submodule_info(all_project, repo_home){
    def lowerstack_patch_info = get_prj_info_by_group_name(all_project, LOWERSTACK_PATCH)
    LOWERSTACK_PATCH_BRANCH = lowerstack_patch_info['branch']
    LOWERSTACK_PATCH_PATH_RELATIVE_TO_REPO_HOME = lowerstack_patch_info['path']
    LOWERSTACK_PATCH_PATH_RELATIVE_TO_WORKSPACE = "${repo_home}\\${LOWERSTACK_PATCH_PATH_RELATIVE_TO_REPO_HOME}"

    def lowerstack_rom_info = get_prj_info_by_group_name(all_project, LOWERSTACK_ROM)
    LOWERSTACK_ROM_BRANCH = lowerstack_rom_info['branch']
    LOWERSTACK_ROM_PATH_RELATIVE_TO_REPO_HOME = lowerstack_rom_info['path']
    LOWERSTACK_ROM_PATH_RELATIVE_TO_WORKSPACE = "${repo_home}\\${LOWERSTACK_ROM_PATH_RELATIVE_TO_REPO_HOME}"

    def rom_info = get_prj_info_by_group_name(all_project, ROM)
    ROM_BRANCH = rom_info['branch']
    ROM_PATH_RELATIVE_TO_REPO_HOME = rom_info['path']
    ROM_PATH_RELATIVE_TO_WORKSPACE = "${repo_home}\\${ROM_PATH_RELATIVE_TO_REPO_HOME}"


    echo "set_submodule_info"
    echo "${LOWERSTACK_PATCH_BRANCH} ${LOWERSTACK_PATCH_PATH_RELATIVE_TO_REPO_HOME} ${LOWERSTACK_PATCH_PATH_RELATIVE_TO_WORKSPACE}"
    echo "${LOWERSTACK_ROM_BRANCH} ${LOWERSTACK_ROM_PATH_RELATIVE_TO_REPO_HOME} ${LOWERSTACK_ROM_PATH_RELATIVE_TO_WORKSPACE}"
    echo "${ROM_BRANCH} ${ROM_PATH_RELATIVE_TO_REPO_HOME} ${ROM_PATH_RELATIVE_TO_WORKSPACE}"

}

def get_submodule_info(all_project, submodule_group_name){
    for(p in all_projects){
        if(p["groups"][0] == submodule_group_name){
            return p
        }
    }
    return ""
}


def set_test_script_name(chip_type){
    jenkins_test_build_name = "jenkins-test-build_${chip_type}.py"
    jenkins_test_script_name = "jenkins-test_${chip_type}.py"
}


def emptyFolder(folderName){
    if (fileExists("${folderName}")){
        dir("${folderName}"){
            echo "clean dir ${folderName}"
            deleteDir()
        }
    }
}


def exe_cmd(cmd){
    echo cmd
    if(isUnix()){
        sh """#!/bin/bash

${cmd}"""

    }else{
        bat cmd
    }
}


def check_test_necessary_by_commit_msg()
{
    def subject = params.GERRIT_CHANGE_SUBJECT
    echo "commit subject: ${subject}"
    def commit_module_submodule = subject.split(":")[0].split('-')
    def module = commit_module_submodule[0]
    def sub_module = ""
    if(commit_module_submodule.length >=2){
        sub_module = commit_module_submodule[1]
    }
    module = module.toLowerCase()
    sub_module = sub_module.toLowerCase()
    if(['spec', 'debug', 'doc'].contains(module)){
        echo "${subject} no need to test"
        return false
    }else if(module == 'script' && sub_module !="ci"){
        echo "${subject} change will not change ci, skip"
        return false
    }else{
        echo "Need to do test for ${module}-${sub_module}"
        return true
    }
}



def set_test_nessary(rom_branch, manifest_len, chip_type){
    def cmd_message = ""
    if(!ROM_BRANCH_CUT_MAP.containsKey(rom_branch.toString())){
        echo "Test for ${rom_branch} is not supported"
        Need_Test = false
        cmd_message = "python \"D:\\admin\\DependTools\\Python Lib\\GerritCommentAdd\\gerrit_comment_add.py\" --gerrit_refspec ${params.GERRIT_REFSPEC} --message \"Test for ${rom_branch} ${chip_type} is not supported\" "
    }

    if(!check_test_necessary_by_commit_msg()){
        cmd_message = "python \"D:\\admin\\DependTools\\Python Lib\\GerritCommentAdd\\gerrit_comment_add.py\" --gerrit_refspec ${params.GERRIT_REFSPEC} --message \"Skip test for ${params.GERRIT_CHANGE_SUBJECT} ${chip_type}\" "
        Need_Test = false
    }
    if([1,'1'].contains(manifest_len) && Need_Test == false){
        cmd_message += " --label_name Test-Verified --label_value +1"
    }
    exe_cmd(cmd_message)
}


def get_prj_info_by_group_name(all_project, proj_group_name){
    for(p in all_project){
        if(p["groups"][0] == proj_group_name){
            return p
        }
    }
    return [:]
}


def do_ci_build(current_project_detail, all_project, manifest_len, HoneyRepo_Home, Manifest_Path, chip_type){
    timestamps{
        stage("CIBuild"){
            echo "do ci build on ${NODE_NAME}"
            set_test_nessary(get_prj_info_by_group_name(all_project, 'sdk')['branch'], manifest_len, chip_type)
            withEnv(["HoneyRepo=${HoneyRepo_Home}", "manifest=${Manifest_Path}", "subGitPath=${HoneyRepo_Home}\\${current_project_detail['path']}", "subGitGroup=${current_project_detail['groups'][0]}"]){
                dir("${HoneyRepo_Home}\\${get_prj_info_by_group_name(all_project, 'sdk')['path']}"){
                    bat "python script/ci-build.py --chipType ${chip_type}"
                }
            }
        }
    }
}


def do_test_build(current_project_detail, all_project, manifest_len, HoneyRepo_Home, Manifest_Path, chip_type){
    timestamps{
        set_test_nessary(get_prj_info_by_group_name(all_project, ROM)['branch'], manifest_len, chip_type)
        if(!Need_Test){
            echo "No need to do test, return"
            return
        }
    }
}


def do_ci_test(current_project_detail, all_project, manifest_len, HoneyRepo_Home, chip_type){
    if(!Need_Test)
    {
        return
    }
}


@NonCPS
def List extractLines(final String content) {
    List myKeys = []
    content.eachLine { line -> 
        myKeys << line
    }
    return myKeys
}

return this
