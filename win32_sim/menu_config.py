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

CFLAGS_BASE = ' -O0 -g -gdwarf-2 -static-libgcc '
CFLAGS_BASE += ' -Werror=undef '
CFLAGS_BASE += ' -Werror=sign-compare '
CFLAGS_BASE += ' -Werror=unused-parameter '
CFLAGS_BASE += ' -Werror=type-limits '
CFLAGS_BASE += ' -Werror=missing-braces '
CFLAGS_BASE += ' -Werror=strict-prototypes '
CFLAGS_BASE += ' -Werror=missing-field-initializers '
CFLAGS_BASE += ' -Werror=empty-body '
CFLAGS_BASE += ' -Werror=unused-variable '
CFLAGS_BASE += ' -Werror=format '
CFLAGS_BASE += ' -Werror=char-subscripts '
CFLAGS_BASE += ' -Werror=parentheses '
CFLAGS_BASE += ' -Werror=implicit-fallthrough '


CFLAGS = CFLAGS_BASE + ' -fno-strict-aliasing -std=gnu11 -Wcomment -Wdouble-promotion'
CXXFLAGS = CFLAGS_BASE + ' -std=c++11 -Wmissing-field-initializers'

LFLAGS = ' -T default.ld'
# LFLAGS += ' -flto' #for arm2d
LFLAGS += ' -pthread -static-libgcc -static-libstdc++ -static'
LFLAGS += ' -Wl,-Map=gui.map'

POST_ACTION = OBJCPY + ' -O binary $TARGET gui.bin\n'
POST_ACTION += SIZE + ' $TARGET \n'


