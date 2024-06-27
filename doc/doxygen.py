#!/usr/bin/env python3

import sys
import os
import shutil


# Change to script directory for consistency
source_path = os.path.dirname(os.path.abspath(__file__))
doc_path = os.path.dirname(source_path)
os.chdir(source_path)

print(source_path)

def cmd(s):
  print("")
  print(s)
  print("-------------------------------------")
  r = os.system(s)
  if r != 0:
    print("Exit build due to previous error")
    exit(-1)


doxyxml_path = os.path.join(doc_path, "doxyxml")
print(doxyxml_path)
os.chdir(doxyxml_path)
print("Running doxygen")
cmd("doxygen Doxyfile")

# BUILD PDF





