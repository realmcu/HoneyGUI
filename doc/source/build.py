#!/usr/bin/env python3

import sys
import os
import re
import shutil


# Change to script directory for consistency
source_path = os.path.dirname(os.path.abspath(__file__))
doc_path = os.path.dirname(source_path)
os.chdir(source_path)

def cmd(s):
  print("")
  print(s)
  print("-------------------------------------")
  r = os.system(s)
  if r != 0:
    print("Exit build due to previous error")
    exit(-1)

def download_tools():
  tool_jira_id = "BTSOCGUI-213"
  print("Release build, need downd tools from {}".format(tool_jira_id))
  download_tool_list = list()
  for file_name in ["Font Convert Tool", "ImageConverter", "RVisualDesigner"]:
    tool_list = Jira().find_packages_from_jira(tool_jira_id, file_name)
    if not tool_list:
      sys.exit("Cannot find {} in {}".format(file_name, tool_jira_id))
    Jira().download_attachements_by_jira_id(tool_jira_id, tool_list[0], source_path, check_status=True)
    download_tool_list.append(tool_list[0])
  return download_tool_list

def add_download_link(download_tool_list):
  if len(download_tool_list) != 3:
    sys.exit("Invalid tool list: {}".format(download_tool_list))
  print("Start add download link")
  index_file = os.path.join(source_path, "tool/index.md")
  if not os.path.exists(index_file):
    sys.exit("Cannot find index file: {}".format(index_file))
  with open(index_file, mode='r', newline='', errors='surrogateescape') as fd:
    stream = fd.read()
    download_link = '''
```eval_rst
Image Convert Tool: :download:`Image Convert Tool <../{}>`

Font Convert Tool: :download:`Font Convert Tool <../{}>`

RVisualDesigner Tool: :download:`RVisualDesigner Tool <../{}>`
```
'''.format(download_tool_list[0], download_tool_list[1], download_tool_list[2])
    stream = re.sub(r'(<!-- Add tool downlod link here -->\n)', lambda objs: objs.group(1) + download_link, stream, count=0, flags=re.M)
  with open(index_file, mode='w+', newline='', errors='surrogateescape') as fd:
    fd.write(stream)


#Enter pipenv
#cmd("pipenv shell")
cmd("pipenv sync")

clean = 0
trans = 0
skip_latex = False
skip_doxygen = False
mutil_version = False
release_build = False
args = sys.argv[1:]
if len(args) >= 1:
  if "clean" in args: clean = 1
  if "skip_latex" in args: skip_latex = True
  if "skip_doxygen" in args: skip_doxygen = True
  if "mutil_version" in args: mutil_version = True
  if "release_build" in args: release_build = True

output_path = "output"
html_out = "html_out"
latex_out = "latex_out"
print("")
print("****************")
print("Building")
print("****************")
if clean:
  os.chdir(doc_path)
  cmd("rm -rf " + output_path)
  cmd("mkdir " + output_path)

if not skip_doxygen:
  doxyxml_path = os.path.join(doc_path, "doxyxml")
  os.chdir(doxyxml_path)
  print("Running doxygen")
  cmd("doxygen Doxyfile")

# BUILD PDF

if not skip_latex:
  # Silly workaround to include the more or less correct PDF download link in the PDF
  #cmd("cp -f " + lang +"/latex/RTKIOT GUI.pdf RTKIOT GUI.pdf | true")
  os.chdir(source_path)
  latex_out_path = os.path.join(doc_path, output_path, latex_out)
  if os.path.exists(latex_out_path):
    shutil.rmtree(latex_out_path)
  os.makedirs(latex_out_path)
  cmd("pipenv run sphinx-build -b latex . {}".format(latex_out_path))

  # Generate PDF
  cmd("cd {} && latexmk -xelatex 'RTKIOT GUI.tex'".format(latex_out_path))
  # Copy the result PDF to the main directory to make it available for the HTML build
  cmd("cd {} && cp -f 'RTKIOT GUI.pdf' '../../source/RTKIOT GUI.pdf'".format(latex_out_path))
else:
  print("skipping latex build as requested")

# BUILD HTML
os.chdir(source_path)
html_out_path = os.path.join(doc_path, output_path, html_out)
if os.path.exists(html_out_path):
  shutil.rmtree(html_out_path)
os.makedirs(html_out_path)
if release_build:
  sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
  from script.jira_helper import *
  download_tool_list = download_tools()
  add_download_link(download_tool_list)
cmd("pipenv run sphinx-build -b html . {}".format(html_out_path))



