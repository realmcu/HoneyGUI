#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Apply the flags to C and C++ compiler settings
set(CONFIG_SOC_SERIES_RTL8773E y)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
	-std=c11 \
    --target=arm-arm-none-eabi \
	-march=armv8m.main -mfpu=fpv5-sp-d16 \
    -mfloat-abi=hard \
    -fno-rtti \
    -flto \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O2 \
    -fno-function-sections \
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
    -Wno-implicit-function-declaration \
    -Werror=undef \
    -Werror=sign-compare \
    -Werror=unused-parameter \
    -Werror=missing-braces \
    -Werror=unused-variable \
    -Werror=format \
    -Werror=char-subscripts \
    -Werror=parentheses \
    -Werror=implicit-fallthrough ")
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-std=c++11 \
    --target=arm-arm-none-eabi \
    -march=armv8m.main -mfpu=fpv5-sp-d16 \
    -mfloat-abi=hard \
    -flto \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O2 \
    -fno-function-sections \
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
    -Wmissing-field-initializers")

    