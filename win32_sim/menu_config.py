import os
import sys

# toolchains options
ARCH='win32'
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


CFLAGS_BASE = ' -O0 -g -gdwarf-2 -static-libgcc -Werror=undef -Werror=sign-compare -Werror=unused-parameter -Werror=type-limits ' \
'-Werror=missing-braces -Werror=strict-prototypes -Werror=missing-field-initializers -Werror=unused-variable -Werror=format -Werror=char-subscripts -Werror=parentheses -Werror=implicit-fallthrough' \

CFLAGS = CFLAGS_BASE + ' -fno-strict-aliasing -std=gnu11 -Wcomment -Wdouble-promotion'
CXXFLAGS = CFLAGS_BASE + ' -std=c++11 -Wmissing-field-initializers'

LFLAGS = ' -T default.ld'
# LFLAGS += ' -flto' #for arm2d
LFLAGS += ' -pthread -static-libgcc -static-libstdc++ -static'
LFLAGS += ' -Wl,-Map=gui.map'

POST_ACTION = OBJCPY + ' -O binary $TARGET gui.bin\n'
POST_ACTION += SIZE + ' $TARGET \n'


