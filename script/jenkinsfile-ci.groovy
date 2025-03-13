import java.text.SimpleDateFormat

// branches defined in BRANCH_CUT_MAP will be tested
jenkins_test_build_name = ""
jenkins_test_script_name = ""


SDK = 'sdk'
SDK_PATH_RELATIVE_TO_REPO_HOME = ''
SDK_PATH_RELATIVE_TO_WORKSPACE = ''
SDK_BRANCH = ''

HONEYGUI_BRANCH_CUT_MAP = [
    "-dev":[
        "cutThrottle" : "BB2CCutTest",
    ]
]

test_module_map = [
    "HoneyGUI_CMD": [
        "label": "HoneyGUI",
        "Test": 1,
        "displayNmae": "HoneyGUI_CMD",
        "testResult": true
    ],
    "HoneyGUI_Simulator": [
        "label": "HoneyGUI",
        "Test": 0,
        "displayNmae": "HoneyGUI_Simulator",
        "testResult": true
    ]
]


Need_Test = true
temp_label_prefix = ""
retryCount = 3

def set_submodule_info(all_project, repo_home){
    def sdk_info = get_prj_info_by_group_name(all_project, SDK)
    SDK_BRANCH = sdk_info['branch']
    SDK_PATH_RELATIVE_TO_REPO_HOME = sdk_info['path']
    SDK_PATH_RELATIVE_TO_WORKSPACE = "${repo_home}\\${SDK_PATH_RELATIVE_TO_REPO_HOME}"

    echo "submodule info"
    echo "${SDK_BRANCH} ${SDK_PATH_RELATIVE_TO_REPO_HOME} ${SDK_PATH_RELATIVE_TO_WORKSPACE}"
}

def get_submodule_info(all_project, submodule_group_name){
    for(p in all_projects){
        if(p["groups"][0] == submodule_group_name){
            return p
        }
    }
    return ""
}


def set_test_script_name(){
    jenkins_test_build_name = "jenkins-test-build.py"
    jenkins_test_script_name = "jenkins-test.py"
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



def set_test_nessary(sdk_branch, manifest_len, chip_type){
    def cmd_message = ""
    if(!HONEYGUI_BRANCH_CUT_MAP.containsKey(sdk_branch.toString())){
        echo "Test for ${sdk_branch} is not supported"
        Need_Test = false
        cmd_message = "python \"D:\\admin\\DependTools\\Python Lib\\GerritCommentAdd\\gerrit_comment_add.py\" --gerrit_refspec ${params.GERRIT_REFSPEC} --message \"Test for ${sdk_branch} ${chip_type} is not supported\" "
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



def is_gui_module(){
    def subject = params.GERRIT_CHANGE_SUBJECT
    def module = subject.split(":")[0].split('-')[0]
    if(module == "GUI"){
        return true
    }else{
        return false
    }
}


def do_ci_build(current_project_detail, all_project, manifest_len, HoneyRepo_Home, Manifest_Path, chip_type){
    timestamps{
        stage("CIBuild"){
            echo "do ci build on ${NODE_NAME}"
            set_test_nessary(get_prj_info_by_group_name(all_project, 'sdk')['branch'], manifest_len, chip_type)
            withEnv(["HoneyRepo=${HoneyRepo_Home}", "manifest=${Manifest_Path}", "subGitPath=${HoneyRepo_Home}\\${current_project_detail['path']}", "subGitGroup=${current_project_detail['groups'][0]}"]){
                dir("${HoneyRepo_Home}\\${get_prj_info_by_group_name(all_project, 'sdk')['path']}"){
                    bat "python script/ci-build.py --chipType ${chip_type}"
                    if(is_gui_module() == true){
                        archiveArtifacts artifacts: "doc/output/html_out/"
                        en_doc_url = BUILD_URL + "artifact/doc/output/html_out/en/index.html"
                        cn_doc_url = BUILD_URL + "artifact/doc/output/html_out/cn/index.html"
                        cmd_message = "python \"D:\\admin\\DependTools\\Python Lib\\GerritCommentAdd\\gerrit_comment_add.py\" --gerrit_refspec ${params.GERRIT_REFSPEC} --message \"EN Doc html: $en_doc_url CN Doc html: $cn_doc_url\" "
                        exe_cmd(cmd_message)
                    }
                }
            }
        }
    }
}


def do_test_build(current_project_detail, all_project, manifest_len, HoneyRepo_Home, Manifest_Path, chip_type){
    timestamps{
        set_test_nessary(get_prj_info_by_group_name(all_project, SDK)['branch'], manifest_len, chip_type)
        if(!Need_Test){
            echo "No need to do test, return"
            return
        }

        stage("CITestBuild"){
            set_submodule_info(all_project, HoneyRepo_Home)
            set_test_script_name()
            build_cmd = "python ${WORKSPACE}/${SDK_PATH_RELATIVE_TO_WORKSPACE}/script/${jenkins_test_build_name} CI"
            for(module in test_module_map.keySet()){
                if(test_module_map[module]["Test"] > 0){
                    build_cmd += " --${module}"
                }
            }
            try{
                emptyFolder('SRC/Dict')
                build_cmd += ' --binDir SRC/Dict'
                withEnv(["HONEYGUI_ROOT=${WORKSPACE}\\${SDK_PATH_RELATIVE_TO_WORKSPACE}"])
                {
                    exe_cmd(build_cmd)
                }
            }catch(err){
                bat "python \"D:\\admin\\DependTools\\Python Lib\\GerritCommentAdd\\gerrit_comment_add.py\" --label_name Test-Verified --label_value -1 --gerrit_refspec USE_ENV --message \"Test Build Error\" "
                error err.message
            }

            // to check
            withEnv(["HoneyRepo_ROOT=${WORKSPACE}\\${HoneyRepo_Home}", "HONEYGUI_ROOT=${WORKSPACE}\\${SDK_PATH_RELATIVE_TO_WORKSPACE}"]){
                copy_script_cmd = "python ${WORKSPACE}/${SDK_PATH_RELATIVE_TO_WORKSPACE}/script/ftp/copy_script.py --testType CI --zipFolder script --localFolder test_script --workDir ${WORKSPACE}/SRC/Dict"
                exe_cmd(copy_script_cmd)
            }


            dir("${SDK_PATH_RELATIVE_TO_WORKSPACE}/script"){
                stash includes: "ftp/download_test_files.py, ftp/ftp_helper.py", name: "Test-Script"
            }

            dir("${WORKSPACE}/${SDK_PATH_RELATIVE_TO_WORKSPACE}/script"){
                stash includes: "base/parse_json.py, \
                                 ftp/ftp_helper.py, ftp/copy_script.py, \
                                report/generate_test_report.py, report/__init__.py, \
                                ", name: "Dump-Files", allowEmpty: true
            }
        }
    }
}


def do_ci_test(current_project_detail, all_project, manifest_len, HoneyRepo_Home, chip_type){
    echo "0"
    if(!Need_Test)
    {
        return
    }
    echo "1"
    set_test_script_name()
    def builders = [:]
    for(module in test_module_map.keySet()){
        def test_module = module
        if(test_module_map[test_module]["Test"] == 0){
            continue
        }
        def build_name = test_module_map[test_module]["displayNmae"]
        echo "2"
        builders["${build_name}"] = {
            throttle([HONEYGUI_BRANCH_CUT_MAP[SDK_BRANCH]["cutThrottle"]]){
                try{
                    echo "3"
                    def is_env_error = true
                    def test_result = false
                    retry(retryCount){
                        if(is_env_error == false){
                            error "Not env error, no need to retry"
                        }
                        def test_node = test_module_map[test_module]["label"]
                        node(test_node){
                            def test_cmd = ""
                            timestamps{
                                stage("${build_name}"){
                                    timeout(120){
                                        echo "execute ${build_name} test on node ${NODE_NAME}"
                                        emptyFolder("SRC")
                                        emptyFolder("script")
                                        emptyFolder("log")
                                        dir("script"){
                                            unstash "Test-Script"
                                        }
                                        try{
                                            exe_cmd("python script/ftp/download_test_files.py --module ${test_module}")
                                        }catch(download_err){
                                            is_env_error = false
                                            error "${download_err}"
                                        }
                                        try{
                                            // to do ,call test
                                            test_cmd = "python ${WORKSPACE}/script/${jenkins_test_script_name} CI 0"
                                            test_cmd += " --module ${test_module}"
                                            exe_cmd(test_cmd)
                                            echo "${test_module} test pass"
                                            test_result = true
                                        }catch(err){
                                            echo "${test_module} test fail ${err.message}"
                                            is_env_error = false
                                            error "execute  ${test_module} dryrun test on ${NODE_NAME} failure"
                                        }finally{
                                            try{
                                                dumplog_cmd = "python ${WORKSPACE}/script/${jenkins_test_script_name} CI 0 --module ${test_module} --step DUMPLOG"
                                                try{
                                                    withEnv(["Test_Result=${test_result}"]){
                                                        exe_cmd(dumplog_cmd + " --dryrun")
                                                    }
                                                }catch(dump_err){
                                                    echo "${dump_err}"
                                                }
                                                stash includes: "SRC/report/jira_info~*.json", name: "Jira-Info-${test_module}", allowEmpty: true
                                            }catch(err){
                                                echo "${err.message}"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }catch(err){
                    test_module_map[test_module]["testResult"] = false
                    error "run ${test_module} failed, ${err}"
                }
            }
        }
    }
    builders["failFast"] = false

    catchError{
        stage("CI Test"){
            parallel builders
        }
    }

    node("keilBuildServerPushJira"){
        stage("generate report"){
            emptyFolder("script")
            emptyFolder("SRC/report")
            dir("script"){
                unstash "Dump-Files"
            }
            for(module in test_module_map.keySet()){
                if(test_module_map[module]["Test"] > 0){
                    unstash "Jira-Info-${module}"
                }
            }

            catchError{
                try{
                    withEnv(["Test_Result=${currentBuild.currentResult}"]){
                        bat "python ${WORKSPACE}/script/report/generate_test_report.py"
                    }
                    archiveArtifacts allowEmptyArchive: true, artifacts: "SRC/report/index.html"
                    if(currentBuild.currentResult == "FAILURE"){
                        exe_cmd("python \"D:\\admin\\DependTools\\Python Lib\\GerritCommentAdd\\gerrit_comment_add.py\" --label_name Test-Verified --label_value -1 --gerrit_refspec ${params.GERRIT_REFSPEC} --message \"HoneyGUI test error\"")
                    }else{
                        echo "test is pass"
                    }
                }catch(error){
                    echo "${error}"
                }
            }

            try{
                remove_script_cmd = "python ${WORKSPACE}/script/ftp/copy_script.py --step removeZip"
                exe_cmd(remove_script_cmd)
            }catch(error){
                echo "${error}"
            }
        }
    }
}


return this
