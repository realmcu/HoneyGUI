import subprocess
import datetime
import os
import sys

def get_git_tag():
    try:
        tag = subprocess.check_output(["git", "describe", "--tags", "--abbrev=0"]).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        tag = "0.0.0"
    return tag

def get_git_branch():
    try:
        branch = subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"]).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        branch = "unknown"
    return branch

def get_git_commit_hash():
    try:
        commit_hash = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        commit_hash = "unknown"
    return commit_hash

def get_git_status():
    try:
        status_output = subprocess.check_output(["git", "status", "--porcelain"]).strip().decode('utf-8')
        if status_output:
            return "dirty"
        else:
            return "clean"
    except subprocess.CalledProcessError:
        return "unknown"

def create_version_header(output_path, tag, branch, commit_hash, repo_status):
    header_content = f"""
#ifndef GUI_VERSION_H
#define GUI_VERSION_H

#define VERSION_TAG "{tag}"
#define VERSION_BRANCH "{branch}"
#define VERSION_COMMIT "{commit_hash}"
#define VERSION_BUILD_DATE "{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
#define VERSION_REPO_STATUS "{repo_status}"

#endif // GUI_VERSION_H
"""
    # Ensure the output directory exists
    os.makedirs(output_path, exist_ok=True)
    output_file_path = os.path.join(output_path, "gui_version.h")
    
    with open(output_file_path, "w") as version_file:
        version_file.write(header_content)
    
    print(f"Generated gui_version.h at {output_file_path} with the following content:")
    print(header_content)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <output_path>")
        sys.exit(1)
    
    output_path = sys.argv[1]

    git_tag = get_git_tag()
    git_branch = get_git_branch()
    git_commit_hash = get_git_commit_hash()
    git_status = get_git_status()
    create_version_header(output_path, git_tag, git_branch, git_commit_hash, git_status)
