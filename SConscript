# for module compiling
import os
from building import *


objs = []
cwd  = GetCurrentDir()

objs = objs + SConscript(cwd + '/gui_engine/SConscript')
objs = objs + SConscript(cwd + '/lvgl/SConscript')


Return('objs')
