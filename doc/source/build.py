#!/usr/bin/env python3

import sys
import os
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


#Enter pipenv
#cmd("pipenv shell")
cmd("pipenv sync")

clean = 0
trans = 0
skip_latex = False
skip_doxygen = False
mutil_version = False
args = sys.argv[1:]
if len(args) >= 1:
  if "clean" in args: clean = 1
  if "skip_latex" in args: skip_latex = True
  if "skip_doxygen" in args: skip_doxygen = True
  if "mutil_version" in args: mutil_version = True

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
cmd("pipenv run sphinx-build -b html . {}".format(html_out_path))



