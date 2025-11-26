import os
import sys

# toolchains options
CROSS_TOOL='gcc'
PLATFORM    = 'gcc'

# Auto-detect platform
if sys.platform.startswith('linux'):
    EXEC_PATH = '/usr/bin'
else:
    EXEC_PATH = r'C:/mingw64/bin'

BSP_LIBRARY_TYPE = None


# toolchains
PREFIX = ''
CC = PREFIX + 'gcc'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
CXX = PREFIX + 'g++'
LINK = PREFIX + 'g++'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

CPATH = ''
LPATH = ''


CFLAGS_BASE = ' -O0 -g -gdwarf-2 -static-libgcc'

CFLAGS = CFLAGS_BASE + ' -fno-strict-aliasing -std=gnu11 -Wdouble-promotion'
CXXFLAGS = CFLAGS_BASE + ' -std=c++11'

LFLAGS = ' -T default.ld'
# LFLAGS += ' -flto' #for arm2d
LFLAGS += ' -pthread'
LFLAGS += ' -Wl,-Map=sdl_sim.map'

POST_ACTION = OBJCPY + ' -O binary $TARGET sdl_sim.bin\n'
POST_ACTION += SIZE + ' $TARGET \n'



