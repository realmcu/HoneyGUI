import os
import sys
import shutil
import subprocess


jenkinsBuild_home = os.environ.get("jenkinsScript_abspath", "")
JenkinsBuild_doc_Dir = os.path.join(jenkinsBuild_home, "release/doc")

sdk_name = os.environ.get("SDK_NAME", "")
branch_name = os.environ.get("BRANCH_NAME", "")
language = os.environ.get("LANGUAGE", "")
doc_src_zip = os.environ.get("DOC_ZIP", "")

#start rst check step
sys.path.append(JenkinsBuild_doc_Dir)
from push import run_command_with_realtime_output
output = os.path.join(jenkinsBuild_home, "rst_check", sdk_name, branch_name, language)
doc_src_path = os.path.join(jenkinsBuild_home, "src", sdk_name, branch_name, language)
shutil.unpack_archive(os.path.join(jenkinsBuild_home, doc_src_zip), doc_src_path, 'zip')
rst_check_config = os.path.join(jenkinsBuild_home, "rst_check_config.py")
check_cmd = ["python", os.path.join(JenkinsBuild_doc_Dir, "doc_check/rst_check.py"), output, doc_src_path, doc_src_path, rst_check_config]
print(check_cmd)
run_command_with_realtime_output(check_cmd)