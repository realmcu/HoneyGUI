# for module compiling
import os
from building import *


objs = []
cwd  = GetCurrentDir()

objs = objs + SConscript(cwd + '/realgui/SConscript')
objs = objs + SConscript(cwd + '/lvgl/SConscript')
objs = objs + SConscript(cwd + '/lvgl_v9/SConscript')


Return('objs')
