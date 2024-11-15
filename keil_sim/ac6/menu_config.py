import os
import sys

# toolchains options
#ARCH='arm'
#CPU='cortex-m4'
CROSS_TOOL='armcc'
PLATFORM    = 'armcc'
EXEC_PATH   = r'C:/mingw64/bin'
BSP_LIBRARY_TYPE = None

BUILD = 'debug'


# toolchains
PREFIX = ''
CC = PREFIX + 'armcc'
AS = PREFIX + 'armasm'
AR = PREFIX + 'armar'
CXX = PREFIX + 'armcc'
LINK = PREFIX + 'armlink'
TARGET_EXT = 'axf'

CPATH = ''
LPATH = ''
if BUILD == 'debug':
    CFLAGS = ' -O0 -g -gdwarf-2 -static-libgcc'
    LFLAGS = ' -pthread'
else:
    CFLAGS = ' '
    LFLAGS = ' '
CXXFLAGS = CFLAGS
POST_ACTION = 'fromelf --bin $TARGET --output gui.bin \nfromelf -z $TARGET'


