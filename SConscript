# for module compiling
import os
from building import *


objs = []
cwd  = GetCurrentDir()

objs = objs + SConscript(cwd + '/realgui/SConscript')
objs += SConscript(cwd + '/example/SConscript')

Return('objs')
