#!/usr/bin/env python3

import sys
import os
import re
import time
import shutil
import json
from conf_common import exclude_patterns


# Change to script directory for consistency
doc_path = os.path.dirname(os.path.abspath(__file__))
os.chdir(doc_path)
gui_path = os.path.join(doc_path, "..")

jenkinsBuild_home = os.environ.get("jenkinsScript_abspath", "")
JenkinsBuild_doc_Dir = os.path.join(jenkinsBuild_home, "release/doc")

version = os.getenv("Release_Version")

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
  index_file = os.path.join(language_path, "tool/index.rst")
  if not os.path.exists(index_file):
    sys.exit("Cannot find index file: {}".format(index_file))
  with open(index_file, mode='r', newline='', errors='surrogateescape') as fd:
    stream = fd.read()
    download_link = '''

Image Convert Tool: :download:`Image Convert Tool <../{}>`

Font Convert Tool: :download:`Font Convert Tool <../{}>`

RVisualDesigner Tool: :download:`RVisualDesigner Tool <../{}>`

'''.format(download_tool_list[0], download_tool_list[1], download_tool_list[2])
    stream = re.sub(r'(.. Add tool downlod link here\n)', lambda objs: objs.group(1) + download_link, stream, count=0, flags=re.M)
  with open(index_file, mode='w+', newline='', errors='surrogateescape') as fd:
    fd.write(stream)

def record_doc_generate_time(index_path):
  nighty_date = time.strftime("%Y-%m-%d", time.localtime())
  record_comment = "\n\n{} update by jenkins\n\n".format(nighty_date)
  index_md = os.path.join(index_path, r'index.rst')
  with open(index_md, mode='r', newline='', errors='surrogateescape') as fd:
      stream = fd.read()
      stream = re.sub(r'(=============\n)', lambda objs: objs.group(1) + record_comment, stream, count=1, flags=re.M)
  with open(index_md, mode='w+', newline='', errors='surrogateescape') as fd:
      fd.write(stream)

def move_contents(src_dir, dst_dir):
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)

    for filename in os.listdir(src_dir):
        src_file_path = os.path.join(src_dir, filename)
        dst_file_path = os.path.join(dst_dir, filename)

        if os.path.isdir(src_file_path):
            move_contents(src_file_path, dst_file_path)
        else:
            shutil.move(src_file_path, dst_file_path)

    if os.path.exists(src_dir) and not os.listdir(src_dir):
        os.rmdir(src_dir)

#Enter pipenv
#cmd("pipenv shell")
cmd("pipenv sync")

clean = 0
trans = 0
skip_latex = False
skip_doxygen = False
mutil_version = False
release_build = False
archive = False
record_time = False
args = sys.argv[1:]
if len(args) >= 1:
  if "clean" in args: clean = 1
  if "skip_latex" in args: skip_latex = True
  if "skip_doxygen" in args: skip_doxygen = True
  if "mutil_version" in args: mutil_version = True
  if "release_build" in args: release_build = True
  if "Archive" in args: archive = True
  if "record_time" in args: record_time = True

output_path = "output"
html_out = output_path + "/html_out"
cn_html_out = r'cn'
en_html_out = r'en'
latex_out = "latex_out"
rst_src_out = "rst_src"
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

rst_src_out = os.path.join(doc_path, output_path, rst_src_out)
if os.path.exists(rst_src_out):
  shutil.rmtree(rst_src_out)
os.makedirs(rst_src_out)

# Add si summary content
if version:
  sys.path.append(JenkinsBuild_doc_Dir)
  from add_ai_summary_src import *
  add_raw_html_to_rst(doc_path, os.path.join(doc_path, "script/ai_summary_config.txt"))

# BUILD HTML
os.chdir(doc_path)
sdk_name = "gui"
en_cn_build = [(" -c ./en en ./{}/{} -D language=en".format(html_out, en_html_out), 'en'), (" -c ./cn cn ./{}/{} -D language=zh_CN".format(html_out, cn_html_out), 'cn')]
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

  if record_time:
    record_doc_generate_time(l_doc_path)

  os.chdir(doc_path)
  if release_build:
    sys.path.append(os.path.join(os.path.dirname(__file__), "../script"))
    from jira_helper import *
    download_tool_list = download_tools(l_doc_path)
    add_download_link(download_tool_list, l_doc_path)
  print("Current path: {}".format(os.getcwd()))
  cmd_line = "pipenv run sphinx-build -b html -j 8"
  ep = ''
  exclude_patterns_ins = []
  exclude_patterns_ins.extend(exclude_patterns)
  if "en" in p:
    html_out_path = os.path.join(doc_path, html_out, en_html_out)
    exclude_patterns_ins.append('cn')
  elif "cn" in p:
    html_out_path = os.path.join(doc_path, html_out, cn_html_out)
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

  # update lnk map file
  if version:
    #generate link map txt
    link_map_file = os.path.join(html_out_path, "link_map.json")
    print(f"Link map file: {link_map_file}")
    if os.path.exists(link_map_file):
      print("Start update url link")
      link = "https://docsqa.realmcu.com/{}/{}/{}/".format(sdk_name, p, version)
      with open(link_map_file, encoding='utf-8', mode='r') as fd:
        json_data = json.load(fd)
      updated_links = dict()
      for pagename, _ in json_data.items():
          updated_links[pagename + ".rst"] = link + json_data[pagename]
      with open(link_map_file, 'w', encoding='utf-8') as fd:
          json.dump(updated_links, fd, ensure_ascii=False, indent=4)
    
    #parse api reference html content
    # rel_api_path = r"API_Reference"
    # api_reference_html_path = os.path.join(html_out_path, rel_api_path)
    # api_reference_content_path = os.path.join(rst_src_out, sdk_name, l, rel_api_path)
    # os.makedirs(api_reference_content_path)
    # sys.path.append(JenkinsBuild_doc_Dir)
    # from parse_html import *
    # parse_html(api_reference_html_path, api_reference_content_path)

    # move rst src file to rst_src_out
    doc_source_path = os.path.join(html_out_path, "doc_source")
    doc_src_zip = "{}-{}-{}-src".format(sdk_name, version, p)
    doc_html_zip = "{}-{}-{}-html".format(sdk_name, version, p)
    dest_copy_path = os.path.join(rst_src_out, sdk_name, p)
    #remove api reference rst src, use html content
    # for f in os.listdir(os.path.join(doc_source_path, rel_api_path)):
    #   api_f = os.path.join(doc_source_path, rel_api_path, f)
    #   if os.path.isfile(api_f):
    #     os.remove(api_f)
    if os.path.exists(doc_source_path):
      move_contents(doc_source_path, dest_copy_path)
      shutil.copy2(link_map_file, dest_copy_path)
      shutil.make_archive(base_name=doc_src_zip, format='zip', root_dir=dest_copy_path)
      shutil.make_archive(base_name=doc_html_zip, format='zip', root_dir=html_out_path)

shutil.copytree(os.path.join(doc_path, html_out), os.path.join(doc_path, html_out + "_" + sdk_name))

if archive:
  os.chdir(gui_path)
  doc_zip_name = "doc-{}".format(os.environ['BRANCH_NAME'].replace("/", "-"))
  shutil.make_archive(base_name=doc_zip_name, format='zip', root_dir=os.path.join(doc_path, html_out))
  print(f"doc zip name: {doc_zip_name}")

