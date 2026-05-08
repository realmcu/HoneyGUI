#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Apply the flags to C and C++ compiler settings
set(CONFIG_SOC_SERIES_RTL87X3J y)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
    -std=gnu11 \
    -march=armv8.1-m.main+dsp+mve+fp.dp \
    -mcpu=cortex-m55 \
    -mabi=aapcs \
    -mfloat-abi=hard \
    -mfpu=fpv5-d16 \
    -mthumb \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -Os \
    -Wall \
    -Werror=undef \
    -Werror=sign-compare \
    -Werror=unused-parameter \
    -Werror=missing-braces \
    -Werror=unused-variable \
    -Werror=format \
    -Werror=char-subscripts \
    -Werror=parentheses \
    -Werror=implicit-fallthrough \
    -Werror=type-limits \
    -Wno-packed \
    -Wno-missing-variable-declarations \
    -Wno-missing-prototypes \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-nonportable-include-path \
    -Wno-reserved-id-macro \
    -Wno-unused-macros \
    -Wno-documentation-unknown-command \
    -Wno-documentation \
    -Wno-license-management \
    -Wno-parentheses-equality \
    -Wno-implicit-function-declaration ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=gnu++11 \
    -march=armv8.1-m.main+dsp+mve+fp.dp \
    -mcpu=cortex-m55 \
    -mabi=aapcs \
    -mfloat-abi=hard \
    -mfpu=fpv5-d16 \
    -mthumb \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -Os \
    -Wall \
    -Wno-packed \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-unused-macros \
    -Werror=undef \
    -Werror=sign-compare \
    -Werror=unused-parameter \
    -Werror=missing-braces \
    -Werror=unused-variable \
    -Werror=format \
    -Werror=char-subscripts \
    -Werror=parentheses \
    -Werror=implicit-fallthrough \
    -Werror=deprecated-copy \
    -Werror=type-limits \
    -Wmissing-field-initializers")
