import os
import sys

# toolchains options
#ARCH='arm'
#CPU='cortex-m4'
CROSS_TOOL='gcc'
PLATFORM    = 'gcc'
EXEC_PATH   = r'C:/mingw64/bin'
BSP_LIBRARY_TYPE = None


# toolchains
PREFIX = ''
CC = PREFIX + 'gcc'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
CXX = PREFIX + 'g++'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

CPATH = ''
LPATH = ''

CFLAGS = ' -O0 -g -gdwarf-2 -static-libgcc'
LFLAGS = ' -T default.ld -pthread'

CXXFLAGS = CFLAGS + ' -std=c++11'
POST_ACTION = OBJCPY + ' -O binary $TARGET gui.bin\n'
POST_ACTION += SIZE + ' $TARGET \n'


