#!/usr/bin/env python3

import sys
import os
import re
import shutil
from conf_common import exclude_patterns


# Change to script directory for consistency
doc_path = os.path.dirname(os.path.abspath(__file__))
os.chdir(doc_path)

def cmd(s):
  print("")
  print(s)
  print("-------------------------------------")
  r = os.system(s)
  if r != 0:
    print("Exit build due to previous error")
    exit(-1)

def download_tools(language_path):
  tool_jira_id = "BTSOCGUI-213"
  print("Release build, need downd tools from {}".format(tool_jira_id))
  download_tool_list = list()
  for file_name in ["ImageConverter", "Font Convert Tool", "RVisualDesigner"]:
    tool_list = Jira().find_packages_from_jira(tool_jira_id, file_name)
    if not tool_list:
      sys.exit("Cannot find {} in {}".format(file_name, tool_jira_id))
    Jira().download_attachements_by_jira_id(tool_jira_id, tool_list[0], language_path, check_status=True)
    download_tool_list.append(tool_list[0])
  return download_tool_list

def add_download_link(download_tool_list, language_path):
  if len(download_tool_list) != 3:
    sys.exit("Invalid tool list: {}".format(download_tool_list))
  print("Start add download link")
  index_file = os.path.join(language_path, "tool/index.md")
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
cn_html_out = r'html_out/cn'
en_html_out = r'html_out/en'
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


# Generate jeba dict
os.chdir(doc_path)
cmd("python generate_jieba_dict.py ./cn ./cn/word_dict.txt")

# BUILD HTML
os.chdir(doc_path)
en_cn_build = [(" -c ./en en ./{}/{} -D language=en".format(output_path, en_html_out), 'en'), (" -c ./cn cn ./{}/{} -D language=zh_CN".format(output_path, cn_html_out), 'cn')]
for l, p in en_cn_build:
  l_doc_path = os.path.join(doc_path, p)
  # BUILD PDF
  if not skip_latex:
    # Silly workaround to include the more or less correct PDF download link in the PDF
    #cmd("cp -f " + lang +"/latex/RTKIOT GUI.pdf RTKIOT GUI.pdf | true")
    os.chdir(l_doc_path)
    latex_out_path = os.path.join(doc_path, output_path, latex_out)
    if os.path.exists(latex_out_path):
      shutil.rmtree(latex_out_path)
    os.makedirs(latex_out_path)
    cmd("pipenv run sphinx-build -b latex . {}".format(latex_out_path))

    # Generate PDF
    cmd("cd {} && latexmk -xelatex 'RTKIOT GUI.tex'".format(latex_out_path))
    # Copy the result PDF to the main directory to make it available for the HTML build
    cmd("cd {} && cp -f 'RTKIOT GUI.pdf' '../../{}/RTKIOT GUI.pdf'".format(latex_out_path, p))
  else:
    print("skipping latex build as requested")

  os.chdir(doc_path)
  if release_build:
    sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
    from script.jira_helper import *
    download_tool_list = download_tools(l_doc_path)
    add_download_link(download_tool_list, l_doc_path)
  print("Current path: {}".format(os.getcwd()))
  cmd_line = "pipenv run sphinx-build -b html -j 8"
  ep = ''
  exclude_patterns_ins = []
  exclude_patterns_ins.extend(exclude_patterns)
  if "en" in l:
    html_out_path = os.path.join(doc_path, output_path, en_html_out)
    exclude_patterns_ins.append('cn')
  elif "cn" in l:
    html_out_path = os.path.join(doc_path, output_path, cn_html_out)
    exclude_patterns_ins.append('en')
  print("exclude_patterns_ins:",exclude_patterns_ins)
  ep = ",".join(exclude_patterns_ins)
  if os.path.exists(html_out_path):
    shutil.rmtree(html_out_path)
  os.makedirs(html_out_path)
  cmd_line += l
  print("ep:", ep)
  cmd_line += " -D exclude_patterns=" + ep
  print(cmd_line)
  cmd(cmd_line)



