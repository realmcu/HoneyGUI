import os
import sys

# toolchains options
ARCH='win32'
CROSS_TOOL='gcc'
PLATFORM    = 'gcc'

# Auto-detect platform: use system gcc on Linux/WSL, MinGW on Windows
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
LINK = PREFIX + 'gcc'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

CPATH = ''
LPATH = ''

# Check if DEBUG_MODE is enabled (set DEBUG_MODE=1 to skip strict checks)
DEBUG_MODE = os.environ.get('DEBUG_MODE', '0') == '1'

CFLAGS_BASE = ' -O0 -g -gdwarf-2 -static-libgcc '

if not DEBUG_MODE:
    CFLAGS_BASE += ' -Werror=undef '
    CFLAGS_BASE += ' -Werror=sign-compare '
    CFLAGS_BASE += ' -Werror=unused-parameter '
    CFLAGS_BASE += ' -Werror=type-limits '
    CFLAGS_BASE += ' -Werror=missing-braces '
    CFLAGS_BASE += ' -Werror=missing-field-initializers '
    CFLAGS_BASE += ' -Werror=empty-body '
    CFLAGS_BASE += ' -Werror=unused-variable '
    CFLAGS_BASE += ' -Werror=format '
    CFLAGS_BASE += ' -Werror=char-subscripts '
    CFLAGS_BASE += ' -Werror=parentheses '
    CFLAGS_BASE += ' -Werror=implicit-fallthrough '

if DEBUG_MODE:
    CFLAGS = CFLAGS_BASE + ' -fno-strict-aliasing -std=gnu11'
    CXXFLAGS = CFLAGS_BASE + ' -std=c++11'
else:
    CFLAGS = CFLAGS_BASE + ' -fno-strict-aliasing -std=gnu11 -Wcomment -Wdouble-promotion -Werror=strict-prototypes'
    CXXFLAGS = CFLAGS_BASE + ' -std=c++11 -Wmissing-field-initializers'

if sys.platform.startswith('linux'):
    LFLAGS = ' -pthread'
    LFLAGS += ' -Wl,-z,noexecstack'
else:
    LFLAGS = ' -pthread -static-libgcc -static-libstdc++ -static'
LFLAGS += ' -Wl,-Map=sdl_sim.map'

POST_ACTION = OBJCPY + ' -O binary $TARGET sdl_sim.bin\n'
POST_ACTION += SIZE + ' $TARGET \n'
